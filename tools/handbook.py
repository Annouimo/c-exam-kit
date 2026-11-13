"""Build course documents from an editable template and structured reference data."""
from pathlib import Path
import json
import re

ROOT=Path(__file__).resolve().parents[1]
TOKEN=re.compile(r'\{\{\s*([A-Za-z_][\w.]*)\s*\}\}')


def read_json(root,path):
    return json.loads((root/path).read_text(encoding='utf-8'))


def load_profile(root=ROOT):
    return read_json(root,'catalog/handbook/profile.json')


def values(data,root=ROOT):
    profile=load_profile(root)
    return dict(profile['metadata'],handbook_version='v'+data['handbook_version'],
        scaffold_version='v'+data['version'],source_filename=f'c_scaffold_v{data["version"]}.c')


def substitute(text,variables):
    def replace(match):
        key=match[1]
        if key not in variables:raise ValueError('Unknown handbook placeholder: '+key)
        return str(variables[key])
    result=TOKEN.sub(replace,text)
    if '{{' in result or '}}' in result:raise ValueError('Malformed or nested handbook placeholder')
    return result


def table(item):
    headers=item['columns'];rows=item['rows']
    if not headers or any(len(row)!=len(headers) for row in rows):raise ValueError('Invalid reference table shape')
    def row(values):return '| '+' | '.join(str(x).replace('|',r'\|').replace('\n',' ') for x in values)+' |'
    return '\n'.join([row(headers),row(['---']*len(headers))]+[row(x) for x in rows])


def topic_catalog(data,profile):
    # Program groups are the single source of names, order and membership.
    out={}
    for g,(key,title) in enumerate(zip(data['group_ids'],data['groups'])):
        lines=['### 相关脚手架：'+title,'']
        for number,item in enumerate(data['components'],1):
            if item['group']==g:
                lines.append(f'- **{number}.** {item["name"]}：{item["summary"]}')
        out['scaffold.'+key]='\n'.join(lines)
    for key,groups_key in [('io','io_groups'),('fragments','fragment_groups')]:
        lines=[]
        for g,title in enumerate(data[groups_key]):
            names=[f'{i+1}「{x["name"]}」' for i,x in enumerate(data[key]) if x['group']==g]
            lines.append('- '+title+'：'+'；'.join(names)+'。')
        if key=='io':lines.append(f'- 其他：{len(data["io"])+1}「{profile["manual_export_label"]}」。')
        out['scaffold.'+key]='\n'.join(lines)
    return out


def chinese_number(n):
    if not 0<n<100:raise ValueError('Chapter numbering supports 1..99 chapters')
    digits='零一二三四五六七八九'
    if n<10:return digits[n]
    return (digits[n//10] if n>=20 else '')+'十'+(digits[n%10] if n%10 else '')


def number_chapters(markdown):
    """Number level-two headings, leaving fenced examples and source templates intact."""
    result=[];count=0;fence=None
    for line in markdown.splitlines(keepends=True):
        marker=re.match(r'^ {0,3}(`{3,}|~{3,})',line)
        if marker:
            token=marker[1]
            if fence is None:fence=token
            elif token[0]==fence[0] and len(token)>=len(fence):fence=None
        if fence is None and line.startswith('## '):
            count+=1;line='## '+chinese_number(count)+'、'+line[3:]
        result.append(line)
    return ''.join(result)


def handbook_outputs(data,root=ROOT):
    profile=load_profile(root);variables=values(data,root)
    references=read_json(root,'catalog/handbook/references.json')
    variables.update({'tables.'+key:table(value) for key,value in references['tables'].items()})
    for key,rows in references['indexes'].items():
        variables['indexes.'+key]='\n'.join('- '+x['label']+'：'+x['text'] for x in rows)
    variables.update(topic_catalog(data,profile))
    template=(root/profile['template']).read_text(encoding='utf-8')
    # Editing annotations belong to the source template, not the reading edition.
    template=re.sub(r'<!--.*?-->','',template,flags=re.S)
    used=TOKEN.findall(template)
    for key in variables:
        if key.startswith('scaffold.') and used.count(key)!=1:
            raise ValueError('Each scaffold section needs exactly one placeholder: '+key)
    group_order=['scaffold.'+key for key in data['group_ids']]
    if [key for key in used if key in group_order]!=group_order:
        raise ValueError('Handbook component groups must follow program order')
    output=substitute(template,variables)
    if profile.get('chapter_numbering')=='chinese':output=number_chapters(output)
    output=re.sub(r'\n{3,}','\n\n',output).strip()+'\n'
    return {substitute(profile['markdown_output'],variables):output}


def expand_default(code):
    for key,value in [('SEPARATOR_LITERAL','" "'),('VALUE_SCAN','"%d"'),('VALUE_PRINT','"%d"'),('VALUE','int')]:
        code=re.sub(r'\b'+key+r'\b',lambda _:value,code)
    for key in ['BODY','LOOP','INPUT','OUTPUT']:
        code=code.replace('@'+key+'@','    /* TODO: 按题目补齐 */\n')
    return code


def all_document_outputs(data,root=ROOT):
    profile=load_profile(root);v=values(data,root)
    result=handbook_outputs(data,root)
    # A full reference manual is another template, independent of the exam book.
    pieces={x['id']:x for x in data['pieces']}
    inputs=[];components=[];fragments=[];group_catalog=[]
    for g,title in enumerate(data['io_groups']):
        inputs.append('### '+title+'\n')
        for i,x in enumerate(data['io']):
            if x['group']!=g:continue
            inputs.extend([f'#### {i+1}. {x["name"]}\n',x['help']+'\n'])
            if x['code']:inputs.append('```c\n'+expand_default(x['code'])+'```\n')
    for g,title in enumerate(data['groups']):
        components.append('### '+title+'\n');group_catalog.append('### '+title+'\n')
        for i,x in enumerate(data['components']):
            if x['group']!=g:continue
            group_catalog.append(f'- **{i+1}.** {x["name"]}：{x["summary"]}')
            components.extend([f'#### {i+1}. {x["name"]}\n',x['note']+'\n'])
            code='#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <math.h>\n\n'
            for cap in data['capacities']:
                if cap['macro'] in x['caps']:
                    code+=f'#define {cap["macro"]} {cap["default"]}\n'
                    if cap['log_macro']:code+=f'#define {cap["log_macro"]} {cap["default"].bit_length()}\n'
            for pid in x['resolved_pieces']:
                p=pieces[pid]
                if p['details']:components.append(p['details']+'\n')
                code+='\n'+re.sub(r'^#define[^\n]*\n','',p['code'],flags=re.M)
            components.append('```c\n'+code+'```\n')
        group_catalog.append('')
    for g,title in enumerate(data['fragment_groups']):
        fragments.append('### '+title+'\n')
        for i,x in enumerate(data['fragments']):
            if x['group']==g:
                fragments.extend([f'#### {i+1}. {x["name"]}\n',x['help']+'\n','```c\n'+expand_default(x['code'])+'```\n'])
    v.update({'manual.inputs':'\n'.join(inputs),'manual.components':'\n'.join(components),
        'manual.fragments':'\n'.join(fragments),'group_catalog':'\n'.join(group_catalog)})
    v.update(topic_catalog(data,profile))
    for template_key,output_key in [('full_manual_template','full_manual_output'),('appendix_template','appendix_output')]:
        template=(root/profile[template_key]).read_text(encoding='utf-8')
        result[substitute(profile[output_key],v)]=substitute(template,v)
    return result
