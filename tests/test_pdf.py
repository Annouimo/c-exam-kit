#!/usr/bin/env python3
"""PDF-only checks; run after building PDF with tools/pdf-requirements.txt installed."""
import html
import json
from pathlib import Path
import re
import subprocess
import sys
import unittest
import tempfile

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from code_highlight import highlight_lines
from build import load_catalog
from handbook import handbook_outputs
PALETTE=json.loads((ROOT/'catalog/handbook/pdf.json').read_text())['code_colors']


class PdfTests(unittest.TestCase):
    def test_highlighting_preserves_code_and_multiline_comments(self):
        source='double pow(double x, double y);\n/* 中文注释\n   int is a comment */\nint r = scanf("%d <&>", &a);\n'
        lines=highlight_lines(source,PALETTE)
        recovered=html.unescape(re.sub(r'<[^>]+>','','\n'.join(lines))).replace('\xa0',' ')
        self.assertEqual(recovered,source)
        self.assertIn('color="'+PALETTE['type']+'"',lines[0])
        self.assertIn('color="'+PALETTE['function']+'">pow',lines[0])
        self.assertIn('color="'+PALETTE['function']+'">scanf',lines[3])
        self.assertIn('color="'+PALETTE['string']+'"',lines[3])
        self.assertNotIn('color="'+PALETTE['type']+'"',lines[2])
        self.assertIn('color="'+PALETTE['comment']+'"',lines[2])
        self.assertIn('&lt;&amp;&gt;',lines[3])
        trailing=highlight_lines('// 输出：42',PALETTE)[0]
        self.assertIn('color="'+PALETTE['comment']+'"',trailing)
        self.assertNotIn('color="'+PALETTE['number']+'"',trailing)

    def test_all_table_signatures_render_without_losing_text(self):
        tables=json.loads((ROOT/'catalog/handbook/references.json').read_text())['tables']
        signatures=[code for table in tables.values() for row in table['rows'] for cell in row for code in re.findall(r'`([^`]+)`',cell)]
        for source in signatures:
            markup='\n'.join(highlight_lines(source,PALETTE))
            recovered=html.unescape(re.sub(r'<[^>]+>','',markup)).replace('\xa0',' ')
            self.assertEqual(recovered,source)

    def test_generated_pdf_text(self):
        data=load_catalog()
        text=subprocess.check_output(['pdftotext',str(ROOT/f'release/c-exam-handbook-v{data["handbook_version"]}.pdf'),'-']).decode()
        markdown=next(iter(handbook_outputs(data).values()))
        for heading in re.findall(r'^## (.+)$',markdown,re.M):self.assertIn(heading,text)
        for word in ['pow','scanf','qsort','malloc','strncpy']:self.assertIn(word,text)
        for removed in ['fpclassify','remainder','remquo','nextafter','tgamma','scalbn','具体签名并不总是']:self.assertNotIn(removed,text)
        self.assertNotIn('{{',text)
    def test_pagination_with_expanded_tables_lists_and_code(self):
        with tempfile.TemporaryDirectory(prefix='pdf-layout-') as tmp:
            root=Path(tmp)
            for extra in [0,7]:
                source=root/f'stress-{extra}.md';target=root/f'stress-{extra}.pdf';report=root/f'stress-{extra}.json'
                rows=['| 函数签名（压力检查） | 说明 |','| --- | --- |']
                rows += [f'| `int function_{i}(const char *text, int capacity);` | 长函数名和说明用于检查跨页表头和单行完整性。 |' for i in range(80+extra)]
                lines=['# Layout fixture','## 长表格','### 函数列表']+rows+['','## 扩充组件清单','### 相关函数']
                lines += [f'- **{i}.** 扩充的组件名称：说明包含较长的函数名称和使用前提，用于检查新增组件后自然分页。' for i in range(45+extra)]
                lines += ['','## 代码片段','### 长代码','```c']+[f'int variable_{i} = {i};' for i in range(45)]+['```','','### 短代码','```c','int answer = 42;','printf("%d", answer);','```']
                source.write_text('\n'.join(lines)+'\n',encoding='utf-8')
                result=subprocess.run([sys.executable,str(ROOT/'tools/build_pdf.py'),'--input-md',str(source),'--output',str(target),'--layout-report',str(report)],capture_output=True,text=True)
                self.assertEqual(result.returncode,0,result.stdout+result.stderr)
                diagnostics=json.loads(report.read_text(encoding='utf-8'))
                self.assertFalse([x for x in diagnostics['warnings'] if x['kind'] in ['orphan_heading','out_of_bounds']])
                text=subprocess.check_output(['pdftotext',str(target),'-']).decode()
                self.assertGreaterEqual(text.count('函数签名（压力检查）'),2)
                self.assertIn(f'function_{79+extra}',text)
                self.assertIn('variable_44',text)



if __name__=='__main__':unittest.main(verbosity=2)
