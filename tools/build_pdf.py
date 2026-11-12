#!/usr/bin/env python3
"""Build a searchable exam handbook PDF with embedded, subsetted Chinese fonts.

Requires pandoc, Noto Sans CJK fonts, reportlab and fonttools. No browser/network.
"""
from pathlib import Path
import html
import json
import re
import subprocess
import tempfile

ROOT=Path(__file__).resolve().parents[1]


def embed_cjk(source,target,characters,bold=False):
    from fontTools.ttLib import TTFont
    from fontTools import subset
    from fontTools.fontBuilder import FontBuilder
    from fontTools.pens.ttGlyphPen import TTGlyphPen
    from fontTools.pens.cu2quPen import Cu2QuPen
    font=TTFont(source,fontNumber=2)
    options=subset.Options();options.recalc_bounds=True
    sub=subset.Subsetter(options=options);sub.populate(unicodes={ord(x) for x in characters}|set(range(32,127)))
    sub.subset(font)
    order=font.getGlyphOrder();glyphset=font.getGlyphSet();glyphs={}
    for name in order:
        pen=TTGlyphPen(glyphset)
        glyphset[name].draw(Cu2QuPen(pen,max_err=1.0,reverse_direction=True))
        glyphs[name]=pen.glyph()
    build=FontBuilder(font['head'].unitsPerEm,isTTF=True)
    build.setupGlyphOrder(order);build.setupCharacterMap(font.getBestCmap())
    build.setupGlyf(glyphs);build.setupHorizontalMetrics(font['hmtx'].metrics)
    build.setupHorizontalHeader(ascent=font['hhea'].ascent,descent=font['hhea'].descent)
    family='Scaffold CJK Bold' if bold else 'Scaffold CJK'
    build.setupNameTable(dict(familyName=family,styleName='Regular',uniqueFontIdentifier=family,
        fullName=family,psName=family.replace(' ','')))
    build.setupOS2(sTypoAscender=font['hhea'].ascent,sTypoDescender=font['hhea'].descent,
        usWinAscent=max(0,font['hhea'].ascent),usWinDescent=max(0,-font['hhea'].descent))
    build.setupPost();build.setupMaxp();build.save(target)


def main():
    from reportlab.pdfbase import pdfmetrics
    from reportlab.pdfbase.ttfonts import TTFont
    from reportlab.lib import colors
    from reportlab.lib.styles import ParagraphStyle
    from reportlab.lib.enums import TA_LEFT
    from reportlab.lib.pagesizes import A4
    from reportlab.platypus import BaseDocTemplate,PageTemplate,Frame,Paragraph,Spacer,LongTable,TableStyle,PageBreak,KeepTogether
    from reportlab.platypus.tableofcontents import TableOfContents
    data=json.loads((ROOT/'catalog/catalog.json').read_text(encoding='utf-8'))
    md=(ROOT/'docs/C语言考试手册.md').read_text(encoding='utf-8')
    ast=json.loads(subprocess.check_output(['pandoc','--from=markdown','--to=json'],input=md.encode()))
    target=ROOT/'release'/f'c-exam-handbook-v{data["handbook_version"]}.pdf';target.parent.mkdir(exist_ok=True)
    chars=md+'•±×−C语言考试手册目录页脚手册版本配套脚手架Annouimo0123456789'
    fonts=Path('/usr/share/fonts/opentype/noto')
    with tempfile.TemporaryDirectory(prefix='c-exam-fonts-') as temp:
        temp=Path(temp)
        for name,file,bold in [('CJK','NotoSansCJK-Regular.ttc',False),('CJKBold','NotoSansCJK-Bold.ttc',True)]:
            out=temp/(name+'.ttf');embed_cjk(fonts/file,out,chars,bold)
            pdfmetrics.registerFont(TTFont(name,out))
        pdfmetrics.registerFont(TTFont('Code','/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf'))
        pdfmetrics.registerFontFamily('CJK',normal='CJK',bold='CJKBold',italic='CJK',boldItalic='CJKBold')
        blue=colors.HexColor('#285773');ink=colors.HexColor('#233143');muted=colors.HexColor('#617080')
        width=A4[0]-88
        styles={}
        for name,size,leading in [('body',9,14),('small',8,12),('cell',7.7,11.2),('code',7.5,11),('h2',13,18),('h3',10,15),('h4',9.5,14),('title',24,32),('toc',9,14)]:
            styles[name]=ParagraphStyle(name,fontName='CJKBold' if name in ['title','h2','h3','h4'] else 'CJK',fontSize=size,leading=leading,textColor=blue if name.startswith('h') else ink,wordWrap='CJK',spaceAfter=6,alignment=TA_LEFT,splitLongWords=True)
        for name in ['h2','h3','h4']:
            styles[name].keepWithNext=True;styles[name].spaceBefore=12
        styles['code'].fontName='Code';styles['code'].backColor=colors.HexColor('#f3f6f8')
        styles['code'].borderPadding=(2,6,2,6);styles['code'].leftIndent=6;styles['code'].rightIndent=6;styles['code'].spaceAfter=0
        styles['cell'].spaceAfter=0
        def code_markup(text):
            return ''.join('<font name="CJK">'+html.escape(part)+'</font>' if any(ord(c)>127 for c in part) else html.escape(part).replace(' ','&#160;') for part in re.findall(r'[^\x00-\x7f]+|[\x00-\x7f]+',text))
        def inline(items):
            out=[]
            for item in items:
                kind=item['t'];value=item.get('c')
                if kind=='Str':out.append(html.escape(value))
                elif kind in ['Space','SoftBreak']:out.append(' ')
                elif kind=='LineBreak':out.append('<br/>')
                elif kind=='Code':out.append('<font name="Code" size="7.5">'+code_markup(value[1])+'</font>')
                elif kind in ['Strong','Emph']:
                    tag='b' if kind=='Strong' else 'i';out.append('<'+tag+'>'+inline(value)+'</'+tag+'>')
                elif kind=='Link':out.append('<link color="#205f86" href="'+html.escape(value[2][0],quote=True)+'">'+inline(value[1])+'</link>')
                elif kind=='Math':
                    math=value[1].replace(r'\pm','±').replace(r'\times','×')
                    if re.search(r'\\[A-Za-z]',math):raise ValueError('Unsupported math: '+math)
                    math=re.sub(r'\^\{([^}]+)\}',r'<super>\1</super>',html.escape(math))
                    out.append(math)
                elif kind=='Span':out.append(inline(value[1]))
                elif kind=='Quoted':out.append('“'+inline(value[1])+'”')
                elif kind=='RawInline':out.append(html.escape(value[1]))
            return ''.join(out)
        def para(text,style='body'):return Paragraph(text or '&#160;',styles[style])
        def blocks_text(blocks):
            return '<br/>'.join(inline(b['c']) for b in blocks if b['t'] in ['Plain','Para'])
        def render(blocks):
            flow=[]
            for block in blocks:
                kind=block['t'];value=block.get('c')
                if kind in ['Para','Plain']:flow.append(para(inline(value)))
                elif kind=='Header':
                    level,attrs,content=value;p=para(inline(content),'h'+str(min(4,max(2,level))))
                    p.section_level=level;p.section_text=re.sub('<[^>]+>','',inline(content))
                    if level==2 and p.section_text=='脚手架目录与使用':flow.append(PageBreak())
                    flow.append(p)
                elif kind=='CodeBlock':
                    code=[Spacer(1,5)]+[para(code_markup(line),'code') for line in value[1].splitlines()]+[Spacer(1,7)]
                    if len(value[1].splitlines())<=30:flow.append(KeepTogether(code))
                    else:flow.extend(code)
                elif kind in ['BulletList','OrderedList']:
                    items=value if kind=='BulletList' else value[1]
                    for i,item in enumerate(items):
                        prefix='• ' if kind=='BulletList' else str(value[0][0]+i)+'. '
                        text=blocks_text(item)
                        if text:flow.append(para(prefix+text))
                        nested=[b for b in item if b['t'] not in ['Plain','Para']]
                        if nested:flow.extend(render(nested))
                elif kind=='BlockQuote':
                    for child in render(value):
                        if isinstance(child,Paragraph):child.style=ParagraphStyle('quote',parent=styles['small'],leftIndent=9,borderPadding=5,backColor=colors.HexColor('#edf3f7'))
                        flow.append(child)
                elif kind=='Table':
                    heads=value[3][1];bodies=value[4];rows=list(heads)
                    for body in bodies:rows.extend(body[2]);rows.extend(body[3])
                    cells=[[para(blocks_text(cell[4]),'cell') for cell in row[1]] for row in rows]
                    count=len(value[2]);ratios={2:[.23,.77],3:[.43,.17,.40],4:[.16,.15,.34,.35]}.get(count,[1/count]*count)
                    if count==4 and '函数' in blocks_text(rows[0][1][0][4]):ratios=[.13,.39,.13,.35]
                    table=LongTable(cells,colWidths=[width*x for x in ratios],repeatRows=len(heads),hAlign='LEFT')
                    table.setStyle(TableStyle([('VALIGN',(0,0),(-1,-1),'TOP'),('GRID',(0,0),(-1,-1),.35,colors.HexColor('#cdd7df')),('BACKGROUND',(0,0),(-1,len(heads)-1),colors.HexColor('#e8f0f5')),('ROWBACKGROUNDS',(0,len(heads)),(-1,-1),[colors.white,colors.HexColor('#f7f9fa')]),('LEFTPADDING',(0,0),(-1,-1),6),('RIGHTPADDING',(0,0),(-1,-1),6),('TOPPADDING',(0,0),(-1,-1),4),('BOTTOMPADDING',(0,0),(-1,-1),4)]))
                    flow.extend([Spacer(1,4),table,Spacer(1,8)])
                elif kind=='HorizontalRule':flow.append(Spacer(1,6))
                else:raise ValueError('Unsupported Markdown block: '+kind)
            return flow
        class Document(BaseDocTemplate):
            def afterFlowable(self,flowable):
                if hasattr(flowable,'section_level'):
                    level=flowable.section_level;key='section-'+str(self.seq.nextf('section'))
                    self.canv.bookmarkPage(key)
                    self.canv.addOutlineEntry(flowable.section_text,key,level=max(0,level-2),closed=False)
                    if level==2:self.notify('TOCEntry',(0,flowable.section_text,self.page,key))
        doc=Document(str(target),pagesize=A4,leftMargin=44,rightMargin=44,topMargin=46,bottomMargin=40,title='C语言考试手册 v'+data['handbook_version'],author='Annouimo')
        def page(canvas,doc):
            canvas.setStrokeColor(colors.HexColor('#ced9e2'));canvas.setLineWidth(.4);canvas.line(44,A4[1]-30,A4[0]-44,A4[1]-30)
            canvas.setFont('CJK',8);canvas.setFillColor(muted)
            canvas.drawString(44,A4[1]-23,'C EXAM KIT / C语言考试手册')
            canvas.drawRightString(A4[0]-44,A4[1]-23,'v'+data['handbook_version'])
            canvas.drawString(44,23,'Annouimo');canvas.drawRightString(A4[0]-44,23,str(doc.page))
        doc.addPageTemplates(PageTemplate(id='handbook',frames=[Frame(44,40,width,A4[1]-86,leftPadding=0,rightPadding=0,topPadding=0,bottomPadding=0)],onPage=page))
        toc=TableOfContents();toc.levelStyles=[styles['toc']]
        story=[para('C语言考试手册','title'),para(f'手册 v{data["handbook_version"]} · 配套脚手架 v{data["version"]} · Annouimo','small'),Spacer(1,6),para('目录','h3'),toc,Spacer(1,12)]+render(ast['blocks'])
        doc.multiBuild(story)
    print(target)


if __name__=='__main__':main()
