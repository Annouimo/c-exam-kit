#!/usr/bin/env python3
"""Build standalone C and scaffold-only docs from catalog + readable C files.

Python 3.9+, standard library only. No C compiler needed for generation.
"""
from pathlib import Path
import argparse
import json
import re
import zipfile

ROOT = Path(__file__).resolve().parents[1]


def source_filename(data):
    return f'c_scaffold_v{data["version"]}.c'


def ascii_c(source):
    """Preserve UTF-8 bytes in C literals without depending on source/exec charset.

    Three-digit octal escapes cannot absorb the next digit (unlike hex escapes).
    Readable Chinese lives in the engineering inputs; generated comments are ASCII.
    """
    token=re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*',re.S)
    def encode(match):
        text=match[0]
        if text.startswith(('"',"'")):
            return ''.join(ch if ord(ch)<128 else ''.join(f'\\{b:03o}' for b in ch.encode('utf-8')) for ch in text)
        return ''.join(ch if ord(ch)<128 else f'\\u{ord(ch):04x}' for ch in text)
    result=token.sub(encode,source)
    result.encode('ascii')  # Reject non-ASCII identifiers or unhandled tokens.
    return result


def cstr(value):
    return json.dumps(value, ensure_ascii=False)


def load_catalog(root=ROOT):
    data = json.loads((root / 'catalog/catalog.json').read_text(encoding='utf-8'))
    pieces = {p['id']: p for p in data['pieces']}
    if len(pieces) != len(data['pieces']):
        raise ValueError('duplicate piece id')
    for collection in ['pieces', 'components', 'io', 'fragments']:
        items = data[collection]
        ids = [x['id'] for x in items]
        if len(ids) != len(set(ids)) or not all(re.fullmatch(r'[A-Za-z_]\w*', x, re.ASCII) for x in ids):
            raise ValueError(f'invalid or duplicate {collection} id')
    caps = data['capacities']
    for key in ['field', 'macro']:
        if len({x[key] for x in caps}) != len(caps):
            raise ValueError(f'duplicate capacity {key}')
    for x in caps:
        if not x['low'] <= x['default'] - x['extra'] <= x['high']:
            raise ValueError(f'invalid capacity default: {x["field"]}')
        for key in ['field', 'macro']:
            if not re.fullmatch(r'[A-Za-z_]\w*', x[key], re.ASCII):
                raise ValueError(f'invalid capacity {key}')
    known_caps = {x['macro'] for x in caps} | {'SEPARATOR'}

    def read_template(path):
        p = (root / path).resolve()
        if not p.is_relative_to(root.resolve()):
            raise ValueError('template path outside project')
        return p.read_text(encoding='utf-8')

    for p in pieces.values():
        p['code'] = read_template(p['source'])
        p['wide_code'] = read_template(p['wide_source']) if p.get('wide_source') else None

    def resolve(keys, active=()):
        ordered = []
        for key in keys:
            if key in active:
                raise ValueError('piece dependency cycle: ' + ' -> '.join((*active, key)))
            if key not in pieces:
                raise ValueError('unknown piece: ' + key)
            for dep in resolve(pieces[key].get('requires', []), (*active, key)) + [key]:
                if dep not in ordered:
                    ordered.append(dep)
        return ordered

    for x in data['components']:
        x['resolved_pieces'] = resolve(x['pieces'])
        text = ''.join(pieces[k]['code'] + (pieces[k]['wide_code'] or '') for k in x['resolved_pieces'])
        # Capacities in shared dependency templates propagate to their consumers.
        x['caps'] = list(dict.fromkeys(x['caps'] + [c for c in sorted(known_caps) if re.search(r'\b' + c + r'\b', text)]))
        x['value_type'] = x['value_type'] or any(pieces[k]['wide_code'] is not None for k in x['resolved_pieces'])
    for collection, groups in [('components', 'groups'), ('io', 'io_groups'), ('fragments', 'fragment_groups')]:
        values = data[collection]
        if [x['group'] for x in values] != sorted(x['group'] for x in values):
            raise ValueError(f'{collection} must follow category order')
        for x in values:
            if not 0 <= x['group'] < len(data[groups]) or not set(x['caps']) <= known_caps:
                raise ValueError(f'invalid group/capability: {x["id"]}')
            if collection != 'components':
                x['code'] = read_template(x['source'])
    for key in ['io', 'fragments']:
        symbols = [x['symbol'] for x in data[key]]
        if len(set(symbols)) != len(symbols) or not all(re.fullmatch(r'[A-Z][A-Z0-9_]*', s) for s in symbols):
            raise ValueError(f'invalid {key} symbols')
    if sum(x.get('default', False) for x in data['io']) != 1:
        raise ValueError('exactly one default I/O template is required')
    if any(x['phase'] not in [0, 1, 2] for x in data['fragments']):
        raise ValueError('invalid fragment phase')
    return data


def catalog_c(data):
    lines = [f'#define APP_VERSION {cstr(data["version"])}', f'#define APP_DESIGNER {cstr(data["designer"])}']
    def array(name, values, integer=False):
        typ = 'int' if integer else 'char *'
        lines.append(f'static const {typ}{" " if integer else ""}{name}[]={{')
        lines.extend((str(v) if integer else 'NULL' if v is None else cstr(v)) + ',' for v in values)
        lines.append('};')
    for name, field in [('pieces','code'),('wide_pieces','wide_code'),('piece_details','details'),('wide_piece_details','wide_details'),('piece_hints','hint')]:
        array(name, [p.get(field) for p in data['pieces']])
    lines += ['typedef struct {const char *name,*short_name,*note,*cap,*help;int group,value_type;int ids['+str(len(data['pieces'])+1)+'];} Component;', 'static const Component components[]={']
    index={p['id']: i for i,p in enumerate(data['pieces'])}
    for x in data['components']:
        strings=[x['name'],x['short_name'],x['note'],','.join(x['caps']),x['help']]
        lines.append('{'+','.join(map(cstr,strings))+f',{x["group"]},{int(x["value_type"])},'+'{'+','.join(map(str,[index[k] for k in x['resolved_pieces']]+[-1]))+'}},')
    lines.append('};')
    array('group_names',data['groups'])
    lines += ['typedef struct {const char *name,*code,*help,*cap,*separator_default;int group,phase,value_type;} Fragment;', 'static const Fragment fragments[]={']
    for x in data['fragments']:
        strings=[x['name'],x['code'],x['help'],','.join(x['caps']),x['default_separator']]
        lines.append('{'+','.join(map(cstr,strings))+f',{x["group"]},{x["phase"]},{int(x["value_type"])}'+'},')
    lines.append('};')
    for name,field in [('io_names','name'),('io_help','help'),('io_codes','code')]:
        array(name,[x[field] for x in data['io']])
    array('io_group',[x['group'] for x in data['io']],integer=True)
    array('io_group_names',data['io_groups'])
    array('fragment_group_names',data['fragment_groups'])
    lines += ['typedef struct {const char *cap;int value_type,functions_only,incomplete,file_write;} IoOptions;', 'static const IoOptions io_options[]={']
    for x in data['io']:
        lines.append('{'+cstr(','.join(x['caps']))+','+','.join(str(int(x[k])) for k in ['value_type','functions_only','incomplete','file_write'])+'},')
    lines.append('};')
    for key in ['io','fragments']:
        lines.append('enum { '+', '.join(x['symbol'] for x in data[key])+' };')
    lines.append('#define DEFAULT_IO '+next(x['symbol'] for x in data['io'] if x['default']))
    return '\n'.join(lines)+'\n'


def config_c(data):
    lines=['typedef struct {', '    int io, selected[COUNT], snippet[FRAGMENTS], wide, separator_set;', '    char separator[128];']
    lines.append('    int '+', '.join(x['field'] for x in data['capacities'])+';')
    lines+=['} Config;', 'typedef struct {const char *macro,*label,*log_macro;size_t offset;int def,low,high,extra;} Setting;', 'static const Setting settings[]={']
    for x in data['capacities']:
        lines.append('{'+','.join(map(cstr,[x['macro'],x['label'],x['log_macro']]))+',offsetof(Config,'+x['field']+'),'+','.join(str(x[k]) for k in ['default','low','high','extra'])+'},')
    lines+=['};','#define SETTINGS ((int)(sizeof settings/sizeof settings[0]))']
    return '\n'.join(lines)


def outputs(data):
    header=(ROOT/'src/platform_headers.h').read_text(encoding='utf-8')
    text_io=(ROOT/'src/text_io.c').read_text(encoding='utf-8')
    runtime=(ROOT/'src/runtime.c').read_text(encoding='utf-8').replace('/* @CONFIG@ */',config_c(data))
    from handbook import all_document_outputs
    name=source_filename(data)
    notice=f'''/******************************************************************************
 *                         STOP - READ THIS FIRST
 *                 COMPILE AND RUN THIS PROGRAM FIRST
 *          DO NOT SEARCH THIS FILE AS A CODE SNIPPET COLLECTION.
 ******************************************************************************
 * C SCAFFOLD GENERATOR v{data["version"]}
 * This file is an INTERACTIVE PROGRAM. Compile it, then RUN the executable.
 * DO NOT just open this file and search for ready-to-copy functions.
 * Compile -> Run -> Select components -> Copy/save the generated C code.
 * Want to SEARCH / COPY functions directly? Open the FULL SCAFFOLD MANUAL.
 *
 * Windows GCC / Dev-C++ (compile as C):
 *   gcc -std=c99 {name} -o c_scaffold.exe -luser32
 * macOS / Linux:
 *   cc -std=c99 {name} -o c_scaffold
 * Run: c_scaffold.exe on Windows, or ./c_scaffold on macOS/Linux.
 * Old IDE output pane: try --plain --stdio-encoding=gbk on Windows,
 * or run the compiled program in a separate console window.
 * This generated file is ASCII-only; its embedded text is UTF-8 bytes.
 * Source and manuals: https://github.com/Annouimo/c-exam-kit/releases
 * Designer: {data["designer"]}
 * Generated by tools/build.py; edit src/, catalog/ and templates/ instead.
 */
'''
    source=ascii_c(notice+header+'\n'+catalog_c(data)+'\n'+text_io+'\n'+runtime)
    return {'dist/'+name:source,**all_document_outputs(data)}


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check',action='store_true',help='fail if generated files differ; write nothing')
    parser.add_argument('--docs-only',action='store_true',help='generate reading editions without rebuilding C source')
    parser.add_argument('--package',action='store_true',help='also create a reproducible source distribution ZIP')
    parser.add_argument('--source-name',action='store_true',help='print the versioned standalone source filename')
    args=parser.parse_args()
    data=load_catalog()
    if args.source_name:
        print(source_filename(data));return
    if args.docs_only and (args.package or args.source_name):parser.error('--docs-only cannot be combined with --package or --source-name')
    stale=[]
    # Keep dist unambiguous for people compiling the downloaded single file.
    for path in ([] if args.docs_only else (ROOT/'dist').glob('*.c')):
        if path.name=='c_template_builder.c' or re.fullmatch(r'c_scaffold_v[0-9.]+\.c',path.name):
            if path.name!=source_filename(data):
                if args.check:stale.append(str(path.relative_to(ROOT))+' (obsolete)')
                else:path.unlink()
    from handbook import all_document_outputs
    rendered=all_document_outputs(data) if args.docs_only else outputs(data)
    for name,text in rendered.items():
        path=ROOT/name
        if args.check:
            # Reading editions are ignored build artifacts; validate generation even in a fresh checkout.
            if (path.exists() and path.read_bytes()!=text.encode('utf-8')) or (not path.exists() and not name.startswith('release/')):stale.append(name)
        else:
            path.parent.mkdir(parents=True,exist_ok=True)
            path.write_bytes(text.encode('utf-8'))
    if stale:parser.exit(1,'Generated files need rebuilding: '+', '.join(stale)+'\n')
    if args.package:
        if args.check:parser.error('--package cannot be combined with --check')
        target=ROOT/'release'/f'c-exam-kit-v{data["version"]}-source.zip'
        target.parent.mkdir(exist_ok=True)
        files=[p for part in ['src','catalog','templates','tools','tests','docs','dist','.github'] for p in (ROOT/part).rglob('*') if p.is_file() and '__pycache__' not in p.parts and p.suffix not in {'.exe','.o','.obj'} and (part!='dist' or p.suffix in {'.c','.sh','.bat'})]
        files += [p for p in [ROOT/'README.md',ROOT/'CHANGELOG.md',ROOT/'.gitignore',ROOT/'.gitattributes'] if p.exists()]
        with zipfile.ZipFile(target,'w',compression=zipfile.ZIP_DEFLATED) as archive:
            for path in sorted(files):
                info=zipfile.ZipInfo('c-exam-kit/'+path.relative_to(ROOT).as_posix(),date_time=(2026,1,1,0,0,0))
                info.compress_type=zipfile.ZIP_DEFLATED
                info.external_attr=(0o100755 if path.suffix=='.sh' else 0o100644)<<16
                archive.writestr(info,path.read_bytes())
        print(target)
    print(f'OK: {len(data["components"])} components, {len(data["io"])} I/O templates, {len(data["fragments"])} fragments')


if __name__=='__main__':
    main()
