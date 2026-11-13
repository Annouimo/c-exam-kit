#!/usr/bin/env python3
"""PDF-only checks; run after building PDF with tools/pdf-requirements.txt installed."""
import html
import json
from pathlib import Path
import re
import subprocess
import sys
import unittest

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


if __name__=='__main__':unittest.main(verbosity=2)
