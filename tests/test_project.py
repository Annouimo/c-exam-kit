#!/usr/bin/env python3
"""Portable integration and boundary tests. Set CC to choose a C compiler."""
from pathlib import Path
import concurrent.futures
import importlib.util
import json
import os
import random
import re
import shutil
import subprocess
import tempfile
import unittest
import sys

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
spec=importlib.util.spec_from_file_location('builder',ROOT/'tools/build.py')
builder=importlib.util.module_from_spec(spec);spec.loader.exec_module(builder)
DATA=builder.load_catalog()
SOURCE=ROOT/'dist'/builder.source_filename(DATA)
CC=os.environ.get('CC') or shutil.which('cc') or shutil.which('gcc')
if not CC:raise SystemExit('A C99 compiler is required; set CC or install GCC/Clang.')
PLATFORM_LIBS=['-luser32'] if os.name=='nt' else []


def run(args,**kwargs):
    p=subprocess.run(list(map(str,args)),capture_output=True,text=True,encoding='utf-8',errors='replace',**kwargs)
    if p.returncode:raise AssertionError(f'{args}\n{p.stdout}\n{p.stderr}')
    return p.stdout


def compile_c(source,target,objects=False):
    args=[CC,'-std=c99','-Wall','-Wextra','-Werror',source,'-o',target]
    if objects:args.insert(1,'-c')
    else:args+=['-lm']+PLATFORM_LIBS
    run(args)


class ProjectTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        run([sys.executable,ROOT/'tools/build.py','--docs-only'])
        cls.temp=tempfile.TemporaryDirectory(prefix='c-exam-test-');cls.d=Path(cls.temp.name)
        cls.exe=cls.d/'builder.exe'
        compile_c(SOURCE,cls.exe)
        cls.prefix='#define main builder_main\n#include '+json.dumps((SOURCE).as_posix())+'\n#undef main\n'

    @classmethod
    def tearDownClass(cls):cls.temp.cleanup()

    def harness(self,body,name):
        p=self.d/(name+'.c');p.write_text(self.prefix+body,encoding='utf-8')
        exe=self.d/(name+'.exe');compile_c(p,exe);return exe

    def test_reproducible_and_handbook_independent(self):
        before=(ROOT/'docs/templates/exam_handbook.md').read_bytes()
        run(['python3',ROOT/'tools/build.py','--check'],cwd=self.d)
        first=builder.outputs(builder.load_catalog());second=builder.outputs(builder.load_catalog())
        self.assertEqual(first,second)
        self.assertEqual(before,(ROOT/'docs/templates/exam_handbook.md').read_bytes())
        appendix=(ROOT/f'release/scaffold-appendix-v{DATA["version"]}.md').read_text(encoding='utf-8')
        self.assertNotIn('|',appendix)
        self.assertEqual(len(re.findall(r'^- \*\*\d+\.\*\* ',appendix,re.M)),len(DATA['components']))
        self.assertIn('https://github.com/Annouimo/c-exam-kit/releases',(ROOT/f'release/c-exam-handbook-v{DATA["handbook_version"]}.md').read_text(encoding='utf-8'))
        self.assertIn(DATA['version'],run([self.exe,'--version']))

    def test_handbook_topics_and_editable_prose(self):
        import copy
        import handbook
        profile=handbook.load_profile()
        generated=handbook.topic_catalog(DATA,profile)
        for i,key in enumerate(DATA['group_ids']):
            entries=re.findall(r'^- \*\*(\d+)\.\*\* (.*)$',generated['scaffold.'+key],re.M)
            self.assertEqual(entries,[(str(n),x['name']+'：'+x['summary']) for n,x in enumerate(DATA['components'],1) if x['group']==i])
            self.assertTrue(generated['scaffold.'+key].startswith('### 相关脚手架：'+DATA['groups'][i]))
        with self.assertRaises(ValueError):handbook.substitute('{{ unknown }}',{})
        with self.assertRaises(ValueError):handbook.table({'columns':['a'],'rows':[['a','b']]})
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp)
            shutil.copytree(ROOT/'catalog',root/'catalog')
            shutil.copytree(ROOT/'docs/templates',root/'docs/templates')
            path=root/profile['template']
            original=path.read_text(encoding='utf-8')
            edited=original.replace('## 数据类型','作者手工添加的说明，不应被构建覆盖。\n\n## 数据类型')
            path.write_text(edited,encoding='utf-8')
            rendered=handbook.handbook_outputs(DATA,root)
            content=next(iter(rendered.values()))
            self.assertIn('作者手工添加的说明，不应被构建覆盖。',content)
            self.assertEqual(path.read_text(encoding='utf-8'),edited)
            self.assertNotIn('{{',content)
            self.assertNotIn('<!--',content)
            self.assertEqual(content.count('作者：Annouimo'),1)
            self.assertEqual(content.count('COMPILE AND RUN'),0)
        self.assertFalse((ROOT/'docs/C语言考试手册.md').exists())

    def test_standard_math_reference_coverage(self):
        reference=json.loads((ROOT/'catalog/handbook/references.json').read_text(encoding='utf-8'))
        common=reference['tables']['math_common']['rows']
        self.assertEqual(len(common),7)
        self.assertEqual({re.search(r'\b(fabs|log|log10|pow|sqrt|ceil|floor)\b',' '.join(row))[0] for row in common},
            {'fabs','log','log10','pow','sqrt','ceil','floor'})
        expected=set('acos asin atan atan2 cos sin tan acosh asinh atanh cosh sinh tanh exp exp2 expm1 frexp ilogb ldexp log1p log2 logb modf cbrt hypot trunc fmod'.split())
        index=' '.join(x['text'] for x in reference['indexes']['math_other'])
        found=set(re.findall(r'`([a-z][a-z0-9]*)(?:\([^`]*\))?`',index))
        self.assertEqual(found,expected)
        self.assertNotIn('math_macros',reference['indexes'])

    def test_chapter_numbering_and_shared_menu_order(self):
        import handbook
        expected=['使用说明','数据类型与常量','输入输出与文本文件','整数与数学计算','数组、排序与查找','字符串处理','其他常用操作']
        template=(ROOT/'docs/templates/exam_handbook.md').read_text(encoding='utf-8')
        content=next(iter(handbook.handbook_outputs(DATA).values()))
        self.assertEqual(re.findall(r'^## (.*)$',template,re.M),expected)
        self.assertEqual(re.findall(r'^## (.*)$',content,re.M),[handbook.chinese_number(i)+'、'+title for i,title in enumerate(expected,1)])
        groups=re.findall(r'^### 相关脚手架：(.*)$',content,re.M)
        self.assertEqual(groups,DATA['groups'])
        self.assertEqual(list(map(int,re.findall(r'^- \*\*(\d+)\.',content,re.M))),list(range(1,len(DATA['components'])+1)))
        self.assertEqual(next(x['group'] for x in DATA['components'] if x['id']=='binary_answer'),DATA['group_ids'].index('search'))
        example='## Intro\n```c\n## example\n```\n## More\n~~~\n## example\n~~~\n'
        self.assertEqual(handbook.number_chapters(example),'## 一、Intro\n```c\n## example\n```\n## 二、More\n~~~\n## example\n~~~\n')
        self.assertEqual(handbook.number_chapters('## Inserted\n'+example).splitlines()[1],'## 二、Intro')
        self.assertEqual([handbook.chinese_number(n) for n in [9,10,11,20,21,99]],['九','十','十一','二十','二十一','九十九'])
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);shutil.copytree(ROOT/'catalog',root/'catalog');shutil.copytree(ROOT/'docs/templates',root/'docs/templates')
            path=root/handbook.load_profile()['template']
            variants=[template.replace('{{ scaffold.math }}',''),template.replace('{{ scaffold.math }}','{{ scaffold.math }}\n{{ scaffold.math }}'),template.replace('{{ scaffold.bits }}','{{ scaffold.math }}').replace('{{ scaffold.math }}\n\n{{ scaffold.bigints }}','{{ scaffold.bits }}\n\n{{ scaffold.bigints }}')]
            for text in variants:
                path.write_text(text,encoding='utf-8')
                with self.assertRaises(ValueError):handbook.handbook_outputs(DATA,root)

    def test_standard_sort_and_search_helpers(self):
        pieces={p['id']:p for p in DATA['pieces']}
        for wide in [False,True]:
            code='\n'.join((pieces[k]['wide_code'] if wide else pieces[k]['code']) for k in ['cmp_int','sort_int_array','bsearch_sorted_int'])
            typ='long long' if wide else 'int'
            low='LLONG_MIN' if wide else 'INT_MIN'
            high='LLONG_MAX' if wide else 'INT_MAX'
            source=self.d/f'sort_helpers_{wide}.c'
            source.write_text('#include <stdlib.h>\n#include <assert.h>\n#include <limits.h>\n'+code+f"""
int main(void) {{
    {typ} a[]={{3,{high},-1,3,{low}}};
    sort_int_array(a,5);
    assert(a[0]=={low} && a[1]==-1 && a[2]==3 && a[3]==3 && a[4]=={high});
    assert(bsearch_sorted_int(a,5,{low})==0);
    assert(bsearch_sorted_int(a,5,{high})==4);
    int i=bsearch_sorted_int(a,5,3);
    assert(i>=0 && i<5 && a[i]==3);
    assert(bsearch_sorted_int(a,5,2)==-1);
    assert(bsearch_sorted_int(a,0,3)==-1);
    sort_int_array(a,0);
    return 0;
}}
""",encoding='utf-8')
            exe=source.with_suffix('.exe');compile_c(source,exe);run([exe])

    def test_ascii_distribution_and_legacy_compiler_charsets(self):
        raw=SOURCE.read_bytes();raw.decode('ascii')
        self.assertIn('COMPILE AND RUN THIS PROGRAM FIRST',raw[:2000].decode())
        self.assertEqual(SOURCE.name,f'c_scaffold_v{DATA["version"]}.c')
        self.assertFalse((ROOT/'dist/c_template_builder.c').exists())
        expected=run([self.exe,'--version'])
        exe=self.d/'legacy_charset.exe'
        if 'clang' not in run([CC,'--version']).lower():
            # GBK compiler flags belong to GCC; Clang accepts only UTF-8 here.
            run([CC,'-std=c99','-Wall','-Wextra','-Werror','-finput-charset=GBK','-fexec-charset=GBK',SOURCE,'-o',exe]+PLATFORM_LIBS)
            self.assertEqual(run([exe,'--version']),expected)
        # Octal byte escapes must not swallow a following ASCII hex/digit character.
        text='const char *s="中A9文"; /* 中文说明 */\n'
        encoded=builder.ascii_c(text);encoded.encode('ascii')
        self.assertIn(r'\344\270\255A9',encoded)

    @unittest.skipUnless(os.name=='nt','Windows redirected legacy-IDE encoding')
    def test_windows_gbk_stdio_and_utf8_saved_source(self):
        out=self.d/'中文脚手架.c'
        data=f'2\n0\n\n2\n{out}\n'.encode('gbk')
        p=subprocess.run([str(self.exe),'--plain','--stdio-encoding=gbk'],input=data,capture_output=True,timeout=15)
        self.assertEqual(p.returncode,0,p.stderr)
        text=p.stdout.decode('gbk')
        self.assertIn('代码已保存',text)
        self.assertTrue(out.read_text(encoding='utf-8').startswith('#include'))
        # A UTF-8 character split across fputc calls still becomes one GBK character.
        exe=self.harness('''int main(void) {
    io_stdio_encoding("gbk");
    const unsigned char s[]={0xe4,0xb8,0xad,0xe6,0x96,0x87,'A',0};
    for(int i=0;s[i];i++)fputc(s[i],stdout);
    return 0;
}
''','bytewise_gbk')
        p=subprocess.run([str(exe)],capture_output=True)
        self.assertEqual(p.stdout.decode('gbk'),'中文A')

    @unittest.skipUnless(os.name=='nt','Native Windows Unicode console')
    def test_windows_unicode_console_ignores_legacy_codepage(self):
        exe=self.harness('''int main(void) {
    FreeConsole();
    if(!AllocConsole())return 77;
    SetConsoleCP(437);SetConsoleOutputCP(437);
    /* AllocConsole preserves inherited pipe handles under STARTF_USESTDHANDLES.
     * Open the actual console devices explicitly for this native-console test. */
    HANDLE output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    HANDLE input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(output==INVALID_HANDLE_VALUE||input==INVALID_HANDLE_VALUE)return 78;
    SetStdHandle(STD_OUTPUT_HANDLE,output);SetStdHandle(STD_INPUT_HANDLE,input);
    COORD origin={0,0};SetConsoleCursorPosition(output,origin);
    const unsigned char s[]={0xe4,0xb8,0xad,0xe6,0x96,0x87,'A',0};
    for(int i=0;s[i];i++)if(fputc(s[i],stdout)==EOF)return 1;
    CHAR_INFO cells[16];COORD size={16,1},zero={0,0};SMALL_RECT rect={0,0,15,0};
    if(!ReadConsoleOutputW(output,cells,size,zero,&rect))return 2;
    int first=0,second=0,ascii=0;
    for(int i=0;i<16;i++) {
        if(cells[i].Char.UnicodeChar==0x4e2d)first=1;
        if(cells[i].Char.UnicodeChar==0x6587)second=1;
        if(cells[i].Char.UnicodeChar=='A')ascii=1;
    }
    if(!first||!second||!ascii)return 3;
    DWORD mode;GetConsoleMode(input,&mode);SetConsoleMode(input,(mode|ENABLE_LINE_INPUT)&~ENABLE_ECHO_INPUT);
    wchar_t line[]={0x4e2d,0x6587,L'\\r'};
    INPUT_RECORD events[3];memset(events,0,sizeof events);
    for(int i=0;i<3;i++) {
        events[i].EventType=KEY_EVENT;events[i].Event.KeyEvent.bKeyDown=TRUE;
        events[i].Event.KeyEvent.wRepeatCount=1;
        events[i].Event.KeyEvent.uChar.UnicodeChar=line[i];
        if(i==2)events[i].Event.KeyEvent.wVirtualKeyCode=VK_RETURN;
    }
    DWORD written;if(!WriteConsoleInputW(input,events,3,&written)||written!=3)return 4;
    char text[32];if(io_read_line(text,sizeof text)!=1)return 5;
    if(strcmp(text,"\\344\\270\\255\\346\\226\\207"))return 6;
    FreeConsole();return 0;
}
''','native_console')
        p=subprocess.run([str(exe)],capture_output=True,timeout=15)
        if p.returncode in [77,78]:self.skipTest('Runner cannot allocate/open a native console')
        self.assertEqual(p.returncode,0,p.stderr)

    def test_all_components_and_skeletons_compile(self):
        folder=self.d/'generated';folder.mkdir()
        # Each component is independent; all components together must deduplicate dependencies.
        exe=self.harness('''static int write_generated(const char *name,Config *c) {
    size_t length=0;
    char *code=generate(c,&length);
    if(!code)return 0;
    FILE *f=fopen(name,"wb");
    if(!f) {free(code);return 0;}
    int ok=fwrite(code,1,length,f)==length;
    if(fclose(f)!=0)ok=0;
    free(code);
    return ok;
}
int main(void) {
    for(int wide=0;wide<2;wide++) {
        for(int id=0;id<COUNT;id++) {
            Config c=defaults();c.io=IO_FUNCTIONS;c.wide=wide;c.selected[id]=1;
            char name[80];sprintf(name,"component_%d_%d.c",id,wide);
            if(!write_generated(name,&c))return 1;
        }
        for(int io=0;io<IO_COUNT;io++) {
            if(io_options[io].incomplete)continue;
            Config c=defaults();c.io=io;c.wide=wide;
            for(int i=0;i<COUNT;i++)c.selected[i]=1;
            char name[80];sprintf(name,"skeleton_%d_%d.c",io,wide);
            if(!write_generated(name,&c))return 1;
        }
        for(int matrix=0;matrix<2;matrix++) {
            Config c=defaults();c.wide=wide;
            c.snippet[matrix?SN_READ_MATRIX:SN_READ_ARRAY]=1;
            c.snippet[matrix?SN_OUTPUT_MATRIX:SN_OUTPUT_ARRAY]=1;
            char name[80];sprintf(name,"fragment_%d_%d.c",matrix,wide);
            if(!write_generated(name,&c))return 1;
        }
    }
    return 0;
}
''','emit_combinations')
        # Simulate unavailable temp directories: assembly is memory-only.
        invalid=self.d/'temp-directory-does-not-exist'
        env=dict(os.environ,TMPDIR=str(invalid),TMP=str(invalid),TEMP=str(invalid))
        run([exe],cwd=folder,env=env)
        paths=list(folder.glob('*.c'))
        def check(p):
            self.assertTrue(p.read_text(encoding='utf-8').startswith('#include'),p.name)
            compile_c(p,p.with_suffix('.o'),objects=True)
        with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:list(pool.map(check,paths))
        self.assertEqual(len(paths),2*len(DATA['components'])+2*(len(DATA['io'])-2)+4)
        print(f'  Strict C99: {len(paths)} generated combinations compiled')

    def test_generate_without_tempfile_permission(self):
        exe=self.harness('''int main(void) {
    Config c=defaults();c.io=IO_FUNCTIONS;c.selected[0]=1;
    size_t length=0;char *source=generate(&c,&length);
    if(!source || !length)return 1;
    if(!strstr(source,"#include") || !strstr(source,"int_to_digits")) {
        free(source);return 2;
    }
    if(strstr(source,"int main(")){free(source);return 3;}
    free(source);return 0;
}
''','no_tmpfile_generation')
        invalid=self.d/'temp-directory-does-not-exist'
        env=dict(os.environ,TMPDIR=str(invalid),TMP=str(invalid),TEMP=str(invalid))
        run([exe],env=env)
        self.assertNotIn('tmpfile(', (ROOT/'src/runtime.c').read_text(encoding='utf-8'))

    def test_algorithm_boundaries(self):
        pieces={p['id']:p for p in DATA['pieces']}
        code='\n'.join(pieces[k]['code'] for k in ['longest_char_run','factorial_basic','rounding'])
        rng=random.Random(20261112)
        tests=[]
        for _ in range(400):
            s=''.join(rng.choice('ab 01+') for _ in range(rng.randrange(60)))
            target=rng.choice('ab 01+z')
            expected=max((len(x) for x in re.findall(re.escape(target)+'+',s)),default=0)
            tests.append(f'assert(longest_char_run({json.dumps(s)},{json.dumps(target)}[0])=={expected});')
        path=self.d/'boundaries.c'
        path.write_text('#include <assert.h>\n#include <math.h>\n'+code+'''\nint main(void) {
assert(factorial_basic(0)==1 && factorial_basic(1)==1);
assert(factorial_basic(5)==120);
assert(factorial_basic(20)==2432902008176640000LL);
assert(longest_char_run("",'a')==0);
assert(longest_char_run("aaabbbaaaa",'a')==4);
assert(longest_char_run("aaabbbaaaa",'b')==3);
assert(longest_char_run("aaabbbaaaa",'z')==0);
assert(round_to_integer(2.5)==3 && round_to_integer(-2.5)==-3);
assert(round_to_integer(-0.49)==0 && round_to_integer(0.5)==1);
assert(fabs(round_decimal(1.234,2)-1.23)<1e-12);
assert(fabs(round_decimal(-1.234,2)+1.23)<1e-12);
assert(round_decimal(-2.5,0)==-3);
'''+ '\n'.join(tests)+'\nreturn 0;\n}\n',encoding='utf-8')
        exe=self.d/'boundaries.exe';compile_c(path,exe);run([exe])

    def test_statistics_large_arrays_and_wide_values(self):
        pieces={p['id']:p for p in DATA['pieces']}
        for wide in [False,True]:
            code=pieces['array_statistics']['wide_code' if wide else 'code']
            typ='long long' if wide else 'int'
            extra='long long b[]={1000000000000LL,1000000000001LL,1000000000002LL,1000000000003LL,1000000000004LL}; assert(array_mean(b,5)==1000000000002.0); assert(array_variance(b,5,array_mean(b,5))==2.0);' if wide else ''
            path=self.d/f'statistics_{wide}.c'
            path.write_text('#include <assert.h>\n#include <math.h>\n'+code+f"""
int main(void) {{
    {typ} sample[]={{1,2,3,4,5}}, symmetric[]={{-2,-1,1,2}}, same[]={{7,7}};
    assert(array_mean(sample,5)==3.0);
    assert(array_variance(sample,5,3.0)==2.0);
    assert(array_mean(symmetric,4)==0.0);
    assert(array_variance(symmetric,4,0.0)==2.5);
    assert(array_variance(same,2,array_mean(same,2))==0.0);
    static {typ} large[200000];
    for(int i=0;i<200000;i++)large[i]=(i%2)?500000:-500000;
    assert(array_mean(large,200000)==0.0);
    assert(fabs(array_variance(large,200000,0.0)-250000000000.0)<1.0);
    for(int i=0;i<200000;i++)large[i]=500000;
    large[0]--;
    double mean=array_mean(large,200000);
    assert(fabs(array_variance(large,200000,mean)-199999.0/40000000000.0)<1e-12);
    {extra}
    return 0;
}}
""",encoding='utf-8')
            exe=path.with_suffix('.exe');compile_c(path,exe);run([exe])

    def test_fixed_output_negative_zero_and_precision(self):
        piece=next(p for p in DATA['pieces'] if p['id']=='print_fixed')['code']
        path=self.d/'fixed_output.c'
        path.write_text('#include <stdio.h>\n#include <stdlib.h>\n#include <assert.h>\n#include <float.h>\n'+piece+"""
int main(void) {
    double values[]={0.0,-0.0,-0.00001,-0.00006,-0.000049,-0.000051,1.23456,-1.23456};
    for(int i=0;i<8;i++){assert(print_fixed(values[i],4));putchar('\\n');}
    assert(print_fixed(-0.1,0));putchar('\\n');
    assert(print_fixed(-1.1,0));putchar('\\n');
    assert(print_fixed(-0.0,15));putchar('\\n');
    assert(!print_fixed(1.0,-1));assert(!print_fixed(1.0,16));
    assert(print_fixed(DBL_MAX,15));putchar('\\n');
    return 0;
}
""",encoding='utf-8')
        exe=path.with_suffix('.exe');compile_c(path,exe);lines=run([exe]).splitlines()
        self.assertEqual(lines[:11],['0.0000','0.0000','0.0000','-0.0001','0.0000','-0.0001','1.2346','-1.2346','0','-1','0.000000000000000'])
        self.assertGreater(len(lines[11]),300)
        self.assertTrue(lines[11].endswith('.000000000000000'))

    def test_string_edits_and_run_boundaries(self):
        piece=next(p for p in DATA['pieces'] if p['id']=='string_edit')['code']
        rng=random.Random(20261114);checks=[];value=''
        for _ in range(250):
            if rng.choice([True,False]) and len(value)<30:
                pos=rng.randrange(len(value)+1);ch=rng.choice('ABC');value=value[:pos]+ch+value[pos:]
                checks.append(f'assert(str_insert_char(s,sizeof s,{pos},\'{ch}\'));')
            else:
                pos=rng.randrange(len(value)+1);count=rng.randrange(len(value)-pos+1)
                value=value[:pos]+value[pos+count:];checks.append(f'assert(str_erase(s,{pos},{count}));')
            checks.append('assert(strcmp(s,'+json.dumps(value)+')==0);')
            if value:
                pos=rng.randrange(len(value));l=pos;r=pos+1
                while l and value[l-1]==value[pos]:l-=1
                while r<len(value) and value[r]==value[pos]:r+=1
                checks.append(f'assert(str_run_bounds(s,{len(value)},{pos},&l,&r)); assert(l=={l} && r=={r});')
        path=self.d/'string_edits.c'
        path.write_text('#include <assert.h>\n#include <string.h>\n'+piece+"""
int main(void) {
    char s[32]="";int l=-1,r=-1;
    char full[4]="ABC";
    assert(!str_insert_char(full,sizeof full,0,'X') && strcmp(full,"ABC")==0);
    assert(!str_insert_char(s,sizeof s,1,'X') && s[0]==0);
    assert(!str_insert_char(s,sizeof s,0,'\\0') && s[0]==0);
    assert(!str_erase(full,4,0) && strcmp(full,"ABC")==0);
    assert(!str_erase(full,1,4) && strcmp(full,"ABC")==0);
    assert(!str_run_bounds(s,0,0,&l,&r) && l==-1 && r==-1);
    assert(str_erase(full,0,3) && full[0]==0);
"""+'\n'.join(checks)+'\nreturn 0;\n}\n',encoding='utf-8')
        exe=path.with_suffix('.exe');compile_c(path,exe);run([exe])

    def test_separator_escaping_and_empty_arrays(self):
        separators=[' ', '+', ', ', '', '??/', '"%\\', ' → ', '\t']
        for index,separator in enumerate(separators):
            # Feed separator as bytes through stdin so the harness itself needs no escaping logic.
            exe=self.harness('''int main(void) {
    Config c=defaults();c.snippet[SN_READ_ARRAY]=1;c.snippet[SN_OUTPUT_ARRAY]=1;
    size_t n=fread(c.separator,1,sizeof c.separator-1,stdin);c.separator[n]=0;
    size_t length=0;char *source=generate(&c,&length);if(!source)return 1;
    int ok=fwrite(source,1,length,stdout)==length;free(source);return ok?0:1;
}
''',f'separator_emit_{index}')
            source=run([exe],input=separator)
            p=self.d/f'separator_result_{index}.c';p.write_text(source,encoding='utf-8')
            target=self.d/f'separator_result_{index}.exe';compile_c(p,target)
            self.assertEqual(run([target],input='4\n1 1 1 1\n'),separator.join(['1']*4)+'\n')
            self.assertEqual(run([target],input='1\n42\n'),'42\n')
            self.assertEqual(run([target],input='0\n'),'\n')

    def test_interactive_configuration(self):
        # Numeric switch, no component/no snippets, save to a file.
        op_num=next(i+1 for i,x in enumerate(DATA['io']) if x['id']=='op_number')
        out=self.d/'operations.c'
        text=run([self.exe,'--plain'],input=f'{op_num}\n0\n\n2\n{out}\n',timeout=10)
        self.assertIn('case 1:',out.read_text(encoding='utf-8'))
        self.assertNotIn('default:',out.read_text(encoding='utf-8'))
        self.assertIn('【本次使用说明',text);self.assertNotIn('源码开始',text)
        # Configure a pure output-expression scaffold; first-use default is '+'.
        frag=next(i+1 for i,x in enumerate(DATA['fragments']) if x['id']=='output_items')
        out=self.d/'expression.c'
        text=run([self.exe,'--plain'],input=f'2\n0\n{frag}\n\n2\n{out}\n',timeout=10)
        self.assertIn('分隔输出',text)
        target=self.d/'expression.exe';compile_c(out,target)
        self.assertEqual(run([target]),'1+1+1+1\n')
        # Function-only selection maps actual current numbering to the new target-char function.
        comp=next(i+1 for i,x in enumerate(DATA['components']) if x['id']=='longest_char_run')
        out=self.d/'char.c'
        text=run([self.exe,'--plain'],input=f'1\n{comp}\n2\n{out}\n',timeout=10)
        self.assertIn('longest_char_run',out.read_text(encoding='utf-8'))
        self.assertNotIn('int main(',out.read_text(encoding='utf-8'))
        self.assertIn('target由调用者传入',text)
        # Export menu number follows template count, never an old hardcoded 16.
        out=self.d/'manual.md'
        run([self.exe,'--plain'],input=f'{len(DATA["io"])+1}\n{out}\n',timeout=10)
        self.assertIn(DATA['version'],out.read_text(encoding='utf-8'))

    def test_catalog_dependencies_and_validation(self):
        root=self.d/'catalog-project';shutil.copytree(ROOT/'templates',root/'templates');(root/'catalog').mkdir()
        path=root/'catalog/catalog.json'
        original=json.loads((ROOT/'catalog/catalog.json').read_text(encoding='utf-8'))
        added={'id':'example_dependency','source':'templates/pieces/example_dependency.c','details':'example','hint':'example','requires':['factorial_basic']}
        (root/added['source']).write_text('long long example_dependency(void) { return factorial_basic(5); }\n',encoding='utf-8')
        original['pieces'].append(added)
        original['components'].append(dict(original['components'][-1],id='example_component',pieces=['example_dependency']))
        path.write_text(json.dumps(original),encoding='utf-8')
        result=builder.load_catalog(root)
        self.assertEqual(result['components'][-1]['resolved_pieces'],['factorial_basic','example_dependency'])
        self.assertIn('example_dependency',builder.catalog_c(result))
        original['pieces'][-1]['requires']=['example_dependency'];path.write_text(json.dumps(original),encoding='utf-8')
        with self.assertRaisesRegex(ValueError,'cycle'):builder.load_catalog(root)
        original['pieces'][-1]['requires']=['missing'];path.write_text(json.dumps(original),encoding='utf-8')
        with self.assertRaisesRegex(ValueError,'unknown piece'):builder.load_catalog(root)

    @unittest.skipIf(os.name=='nt','Tests the POSIX clipboard pipe, separate from native Windows clipboard')
    def test_clipboard_pipe_success_and_failure(self):
        # Compile an executable mock backend; no shell or system clipboard is involved.
        p=self.d/'clipboard_backend.c'
        p.write_text('''#include <stdio.h>
#include <stdlib.h>
int main(void) {
    if(getenv("MOCK_FAIL"))return 1;
    FILE *f=fopen(getenv("MOCK_CLIPBOARD"),"wb");if(!f)return 1;
    int ch;while((ch=getchar())!=EOF)fputc(ch,f);
    return fclose(f)!=0;
}
''',encoding='utf-8')
        backend=self.d/'clipboard_backend';compile_c(p,backend)
        source=(SOURCE).read_text(encoding='utf-8').replace('"/usr/bin/pbcopy"',json.dumps(backend.as_posix()))
        p=self.d/'mock_mac_builder.c';p.write_text(source,encoding='utf-8')
        exe=self.d/'mock_mac_builder';run([CC,'-std=c99','-Wall','-Wextra','-Werror','-D__APPLE__',p,'-o',exe])
        env=os.environ.copy();env['MOCK_CLIPBOARD']=str(self.d/'copied.c')
        text=run([exe,'--plain'],input='2\n0\n\n\n',env=env,timeout=10)
        self.assertIn('【复制成功】',text);self.assertNotIn('源码开始',text)
        copied=Path(env['MOCK_CLIPBOARD']).read_text(encoding='utf-8')
        self.assertTrue(copied.startswith('#include'))
        self.assertNotIn('设计者',copied)
        self.assertIn('int main(',copied);self.assertNotIn('本次使用说明',copied)
        env['MOCK_FAIL']='1'
        text=run([exe,'--plain'],input='2\n0\n\n\n0\n',env=env,timeout=10)
        self.assertIn('【未复制成功】',text);self.assertIn('【代码尚未取出】',text)
        self.assertNotIn('【复制成功】',text)

    @unittest.skipIf(os.name=='nt','PTY is POSIX-only; native Windows compile is covered by CI')
    def test_terminal_menus_and_pause(self):
        import pty,fcntl,termios,struct,select,unicodedata
        for width in [76,100,120,160]:
            master,slave=pty.openpty()
            fcntl.ioctl(slave,termios.TIOCSWINSZ,struct.pack('HHHH',50,width,0,0))
            env=os.environ.copy();env['TERM']='xterm'
            proc=subprocess.Popen([str(self.exe),'--plain'],stdin=slave,stdout=slave,stderr=slave,env=env);os.close(slave)
            os.write(master,b'1\n');buf=b''
            while '组件：'.encode() not in buf:
                ready,_,_=select.select([master],[],[],8);self.assertTrue(ready);buf+=os.read(master,65536)
            text=buf.decode();menu=text.split('已选输入输出：')[1].split('编号可跨类')[0].splitlines()[1:]
            ids=[]
            for line in menu:
                ids.extend(map(int,re.findall(r'(?:^|\s)(\d{1,2}) (?=[\u4e00-\u9fff])',line)))
                display=sum(2 if unicodedata.east_asian_width(ch) in 'WF' else 1 for ch in line.replace('\r',''))
                self.assertLessEqual(display,width,line)
            self.assertEqual(sorted(ids),list(range(1,len(DATA['components'])+1)))
            os.write(master,b'-1\n');remaining=b''
            while '按回车退出'.encode() not in remaining:
                ready,_,_=select.select([master],[],[],8);self.assertTrue(ready);remaining+=os.read(master,65536)
            self.assertIsNone(proc.poll(),'Interactive run must pause before closing')
            os.write(master,b'\n');proc.wait(timeout=5);os.close(master)


if __name__=='__main__':unittest.main(verbosity=2)
