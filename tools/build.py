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


def expand_default(code):
    for key,value in [('SEPARATOR_LITERAL','" "'),('VALUE_SCAN','"%d"'),('VALUE_PRINT','"%d"'),('VALUE','int')]:
        code=re.sub(r'\b'+key+r'\b',lambda _:value,code)
    for key in ['BODY','LOOP','INPUT','OUTPUT']:
        code=code.replace('@'+key+'@','    /* TODO: 按题目补齐 */\n')
    return code


def docs(data):
    appendix=['## 脚手架目录与使用\n', f'配套脚手架版本：v{data["version"]}。\n', '先选输入输出骨架，再选函数组件与可选main片段、配置类型和容量。默认复制纯源码到剪贴板，控制台显示本次所选功能的说明；输入 `c` 查看源码、`s` 另存文件，回车退出。复制失败可改为保存文件。按 `TODO` 补齐题目逻辑，函数需要自己调用；仅函数模式适合接入已有程序。\n']
    for g,title in enumerate(data['groups']):
        appendix.append('### '+title+'\n')
        appendix.extend(f'- {i+1}. {x["name"]}：{x["summary"]}' for i,x in enumerate(data['components']) if x['group']==g)
        appendix.append('')
    for key,groups,title in [('io','io_groups','程序的输入输出方式'),('fragments','fragment_groups','main里的可选代码片段')]:
        appendix.append('### '+title+'\n')
        for g,group in enumerate(data[groups]):
            rows=[f'{i+1}「{x["name"]}」' for i,x in enumerate(data[key]) if x['group']==g]
            appendix.append('- '+group+'：'+'；'.join(rows)+'。')
        if key=='io':appendix.append(f'- 其他：{len(data["io"])+1}「导出全量脚手架手册」。')
        appendix.append('')
    pieces={x['id']:x for x in data['pieces']}
    full=[f'# C语言全量脚手架手册 v{data["version"]}\n',f'设计者：{data["designer"]}\n', '离线固定模板装配工具。选择骨架、组件、可选main片段并配置，生成后自行补齐题意、函数调用和输入输出。以下默认为int；需要时在程序选择long long。生成代码使用math.h时，GCC/Clang通常需要链接 `-lm`。\n']
    for key,groups,title in [('io','io_groups','输入输出骨架'),('components','groups','函数组件'),('fragments','fragment_groups','main里的可选片段')]:
        full.append('## '+title+'\n')
        for g,group in enumerate(data[groups]):
            full.append('### '+group+'\n')
            for i,x in enumerate(data[key]):
                if x['group']!=g:continue
                full.extend([f'#### {i+1}. {x["name"]}\n',x.get('note',x['help'])+'\n'])
                if key=='components':
                    code='#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <math.h>\n\n'
                    for cap in data['capacities']:
                        if cap['macro'] in x['caps']:
                            code+=f'#define {cap["macro"]} {cap["default"]}\n'
                            if cap['log_macro']:code+=f'#define {cap["log_macro"]} {cap["default"].bit_length()}\n'
                    for pid in x['resolved_pieces']:
                        p=pieces[pid]
                        if p['details']:full.append(p['details']+'\n')
                        code+='\n'+re.sub(r'^#define[^\n]*\n','',p['code'],flags=re.M)
                    full.append('```c\n'+code+'```\n')
                elif x['code']:
                    full.extend([('' if key=='io' else x['help']+'\n'),'```c\n'+expand_default(x['code'])+'```\n'])
    full.extend(['## 自测与提交检查\n','- 检查TODO、函数调用、0/1下标、数组容量、多组状态重置。','- 试空输入、单元素、重复值、全负数、末行无换行。','- 核对输出小数位、分隔符和换行；分隔符只在两项之间输出。','- 基础阶乘只支持0..20；更大结果使用大整数。','- 四舍五入恰好一半时远离0，严格十进制题不要直接依赖二进制浮点。',''])
    return '\n'.join(appendix), '\n'.join(full)


def outputs(data):
    header=(ROOT/'src/platform_headers.h').read_text(encoding='utf-8')
    runtime=(ROOT/'src/runtime.c').read_text(encoding='utf-8').replace('/* @CONFIG@ */',config_c(data))
    appendix,full=docs(data)
    return {'dist/c_template_builder.c':'/* Generated by tools/build.py; edit src/, catalog/ and templates/ instead. */\n'+header+'\n'+catalog_c(data)+'\n'+runtime,'docs/脚手架附录.md':appendix,'docs/全量脚手架手册.md':full}


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check',action='store_true',help='fail if generated files differ; write nothing')
    parser.add_argument('--package',action='store_true',help='also create a reproducible source distribution ZIP')
    args=parser.parse_args()
    data=load_catalog()
    stale=[]
    for name,text in outputs(data).items():
        path=ROOT/name
        if args.check:
            if not path.exists() or path.read_bytes()!=text.encode('utf-8'):stale.append(name)
        else:
            path.parent.mkdir(parents=True,exist_ok=True)
            path.write_bytes(text.encode('utf-8'))
    if stale:parser.exit(1,'Generated files need rebuilding: '+', '.join(stale)+'\n')
    if args.package:
        if args.check:parser.error('--package cannot be combined with --check')
        target=ROOT/'release'/f'c-exam-kit-v{data["version"]}-source.zip'
        target.parent.mkdir(exist_ok=True)
        files=[p for part in ['src','catalog','templates','tools','tests','docs','dist','.github'] for p in (ROOT/part).rglob('*') if p.is_file() and '__pycache__' not in p.parts]
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
