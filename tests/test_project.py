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

ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('builder',ROOT/'tools/build.py')
builder=importlib.util.module_from_spec(spec);spec.loader.exec_module(builder)
DATA=builder.load_catalog()
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
        cls.temp=tempfile.TemporaryDirectory(prefix='c-exam-test-');cls.d=Path(cls.temp.name)
        cls.exe=cls.d/'builder.exe'
        compile_c(ROOT/'dist/c_template_builder.c',cls.exe)
        cls.prefix='#define main builder_main\n#include '+json.dumps((ROOT/'dist/c_template_builder.c').as_posix())+'\n#undef main\n'

    @classmethod
    def tearDownClass(cls):cls.temp.cleanup()

    def harness(self,body,name):
        p=self.d/(name+'.c');p.write_text(self.prefix+body,encoding='utf-8')
        exe=self.d/(name+'.exe');compile_c(p,exe);return exe

    def test_reproducible_and_handbook_independent(self):
        before=(ROOT/'docs/C语言考试手册.md').read_bytes()
        run(['python3',ROOT/'tools/build.py','--check'],cwd=self.d)
        first=builder.outputs(builder.load_catalog());second=builder.outputs(builder.load_catalog())
        self.assertEqual(first,second)
        self.assertEqual(before,(ROOT/'docs/C语言考试手册.md').read_bytes())
        appendix=(ROOT/'docs/脚手架附录.md').read_text(encoding='utf-8')
        self.assertNotIn('|',appendix)
        self.assertEqual(len(re.findall(r'^- \d+\. ',appendix,re.M)),len(DATA['components']))
        self.assertIn('https://github.com/Annouimo/c-exam-kit/releases',before.decode())
        self.assertIn(DATA['version'],run([self.exe,'--version']))

    def test_all_components_and_skeletons_compile(self):
        folder=self.d/'generated';folder.mkdir()
        # Each component is independent; all components together must deduplicate dependencies.
        exe=self.harness('''int main(void) {
    for(int wide=0;wide<2;wide++) {
        for(int id=0;id<COUNT;id++) {
            Config c=defaults();c.io=IO_FUNCTIONS;c.wide=wide;c.selected[id]=1;
            char name[80];sprintf(name,"component_%d_%d.c",id,wide);
            FILE *f=fopen(name,"w");if(!f)return 1;emit(f,&c);fclose(f);
        }
        for(int io=0;io<IO_COUNT;io++) {
            if(io_options[io].incomplete)continue;
            Config c=defaults();c.io=io;c.wide=wide;
            for(int i=0;i<COUNT;i++)c.selected[i]=1;
            char name[80];sprintf(name,"skeleton_%d_%d.c",io,wide);
            FILE *f=fopen(name,"w");if(!f)return 1;emit(f,&c);fclose(f);
        }
        for(int matrix=0;matrix<2;matrix++) {
            Config c=defaults();c.wide=wide;
            c.snippet[matrix?SN_READ_MATRIX:SN_READ_ARRAY]=1;
            c.snippet[matrix?SN_OUTPUT_MATRIX:SN_OUTPUT_ARRAY]=1;
            char name[80];sprintf(name,"fragment_%d_%d.c",matrix,wide);
            FILE *f=fopen(name,"w");if(!f)return 1;emit(f,&c);fclose(f);
        }
    }
    return 0;
}
''','emit_combinations')
        run([exe],cwd=folder)
        paths=list(folder.glob('*.c'))
        def check(p):compile_c(p,p.with_suffix('.o'),objects=True)
        with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:list(pool.map(check,paths))
        self.assertEqual(len(paths),2*len(DATA['components'])+2*(len(DATA['io'])-2)+4)
        print(f'  Strict C99: {len(paths)} generated combinations compiled')

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

    def test_separator_escaping_and_empty_arrays(self):
        separators=[' ', '+', ', ', '', '"%\\', ' → ', '\t']
        for index,separator in enumerate(separators):
            # Feed separator as bytes through stdin so the harness itself needs no escaping logic.
            exe=self.harness('''int main(void) {
    Config c=defaults();c.snippet[SN_READ_ARRAY]=1;c.snippet[SN_OUTPUT_ARRAY]=1;
    size_t n=fread(c.separator,1,sizeof c.separator-1,stdin);c.separator[n]=0;
    emit(stdout,&c);return 0;
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
        source=(ROOT/'dist/c_template_builder.c').read_text(encoding='utf-8').replace('"/usr/bin/pbcopy"',json.dumps(backend.as_posix()))
        p=self.d/'mock_mac_builder.c';p.write_text(source,encoding='utf-8')
        exe=self.d/'mock_mac_builder';run([CC,'-std=c99','-Wall','-Wextra','-Werror','-D__APPLE__',p,'-o',exe])
        env=os.environ.copy();env['MOCK_CLIPBOARD']=str(self.d/'copied.c')
        text=run([exe,'--plain'],input='2\n0\n\n\n',env=env,timeout=10)
        self.assertIn('【复制成功】',text);self.assertNotIn('源码开始',text)
        copied=Path(env['MOCK_CLIPBOARD']).read_text(encoding='utf-8')
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
