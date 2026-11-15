#!/usr/bin/env python3
"""Build a searchable exam handbook PDF with embedded, subsetted Chinese fonts.

Requires pandoc, Noto Sans CJK fonts, reportlab and fonttools. No browser/network.
"""
from pathlib import Path
import argparse
import html
import json
import re
import subprocess
import tempfile

ROOT=Path(__file__).resolve().parents[1]


def embed_cjk(source,target,characters,bold=False,font_number=2):
    from fontTools.ttLib import TTFont
    from fontTools import subset
    from fontTools.fontBuilder import FontBuilder
    from fontTools.pens.ttGlyphPen import TTGlyphPen
    from fontTools.pens.cu2quPen import Cu2QuPen
    font=TTFont(source,fontNumber=font_number)
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
    from build import load_catalog
    from handbook import values,substitute,handbook_outputs
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input-md',type=Path,help='render a separate Markdown fixture without changing course data')
    parser.add_argument('--output',type=Path,help='override PDF output')
    parser.add_argument('--layout-report',type=Path,help='write pagination diagnostics')
    parser.add_argument('--preview',action='store_true',help='mark this PDF as an unpublished preview')
    args=parser.parse_args()
    data=load_catalog();meta=values(data)
    pdf=json.loads((ROOT/'catalog/handbook/pdf.json').read_text(encoding='utf-8'))
    generated=handbook_outputs(data)
    md=args.input_md.read_text(encoding='utf-8') if args.input_md else next(iter(generated.values()))
    ast=json.loads(subprocess.check_output(['pandoc','--from=markdown','--to=json'],input=md.encode()))
    metadata=[];body=[]
    for block in ast['blocks']:
        if block['t']=='Header' and block['c'][0]==1:continue
        if block['t']=='Para' and len(metadata)<pdf['metadata_paragraphs'] and not body:metadata.append(block)
        else:body.append(block)
    target=args.output or ROOT/substitute(pdf['output'],meta);target.parent.mkdir(parents=True,exist_ok=True)
    labels={k:substitute(pdf[k],meta) for k in ['header_left','header_right','footer_left','contents_title']}
    chars=md+'•±×−0123456789'+''.join(labels.values())+pdf['preview_label']
    fonts=Path(pdf['font_directory']);margins=pdf['margins']
    with tempfile.TemporaryDirectory(prefix='c-exam-fonts-') as temp:
        temp=Path(temp)
        for name,file,bold in [('CJK','NotoSansCJK-Regular.ttc',False),('CJKBold','NotoSansCJK-Bold.ttc',True)]:
            out=temp/(name+'.ttf');embed_cjk(fonts/file,out,chars,bold,pdf['font_collection_index'])
            pdfmetrics.registerFont(TTFont(name,out))
        pdfmetrics.registerFont(TTFont('Code',pdf['mono_font']))
        pdfmetrics.registerFontFamily('CJK',normal='CJK',bold='CJKBold',italic='CJK',boldItalic='CJKBold')
        palette={k:colors.HexColor(v) for k,v in pdf['colors'].items()}
        muted=palette['muted'];width=A4[0]-margins['left']-margins['right']
        styles={}
        for name,options in pdf['styles'].items():
            options=options.copy()
            for key in ['textColor','backColor']:
                if key in options:options[key]=colors.HexColor(options[key])
            if name in ['h2','h3','h4']:options['keepWithNext']=False
            styles[name]=ParagraphStyle(name,wordWrap='CJK',alignment=TA_LEFT,splitLongWords=True,**options)
        from code_highlight import highlight_lines
        def code_markup(text):return '<br/>'.join(highlight_lines(text,pdf['code_colors']))
        def inline(items):
            out=[]
            for item in items:
                kind=item['t'];value=item.get('c')
                if kind=='Str':out.append(html.escape(value))
                elif kind in ['Space','SoftBreak']:out.append(' ')
                elif kind=='LineBreak':out.append('<br/>')
                elif kind=='Code':out.append(f'<font name="Code" size="{pdf["inline_code_size"]}">'+code_markup(value[1])+'</font>')
                elif kind in ['Strong','Emph']:
                    tag='b' if kind=='Strong' else 'i';out.append('<'+tag+'>'+inline(value)+'</'+tag+'>')
                elif kind=='Link':out.append('<link color="'+pdf['colors']['link']+'" href="'+html.escape(value[2][0],quote=True)+'">'+inline(value[1])+'</link>')
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
                    flow.append(p)
                elif kind=='CodeBlock':
                    code=[Spacer(1,5)]+[para(line,'code') for line in highlight_lines(value[1],pdf['code_colors'])]+[Spacer(1,7)]
                    if len(value[1].splitlines())<=pdf['pagination']['short_code_lines']:
                        group=KeepTogether(code);group.min_start_height=sum(x.wrap(width,A4[1])[1]+x.getSpaceBefore()+x.getSpaceAfter() for x in code)
                        flow.append(group)
                    else:flow.extend(code)
                elif kind in ['BulletList','OrderedList']:
                    items=value if kind=='BulletList' else value[1]
                    list_start=len(flow)
                    for i,item in enumerate(items):
                        prefix='• ' if kind=='BulletList' else str(value[0][0]+i)+'. '
                        text=blocks_text(item)
                        if text:flow.append(para(prefix+text,'list'))
                        nested=[b for b in item if b['t'] not in ['Plain','Para']]
                        if nested:flow.extend(render(nested))
                    paragraphs=[x for x in flow[list_start:] if isinstance(x,Paragraph)]
                    if len(paragraphs)>=2:
                        paragraphs[0].keepWithNext=True
                        paragraphs[-2].keepWithNext=True
                        paragraphs[0].min_start_height=sum(x.wrap(width,A4[1])[1]+x.getSpaceAfter() for x in paragraphs[:2])
                elif kind=='BlockQuote':
                    for child in render(value):
                        if isinstance(child,Paragraph):child.style=ParagraphStyle('quote',parent=styles['small'],leftIndent=9,borderPadding=5,backColor=palette['quote'])
                        flow.append(child)
                elif kind=='Table':
                    heads=value[3][1];bodies=value[4];rows=list(heads)
                    for body in bodies:rows.extend(body[2]);rows.extend(body[3])
                    cells=[[para(blocks_text(cell[4]),'cell') for cell in row[1]] for row in rows]
                    count=len(value[2]);ratios=pdf['table_widths'].get(str(count),[1/count]*count)
                    for rule in pdf['table_width_rules']:
                        if count==rule['columns'] and rule['first_header_contains'] in blocks_text(rows[0][1][0][4]):ratios=rule['widths']
                    table=LongTable(cells,colWidths=[width*x for x in ratios],repeatRows=len(heads),hAlign='LEFT')
                    table.setStyle(TableStyle([('VALIGN',(0,0),(-1,-1),'TOP'),('GRID',(0,0),(-1,-1),.35,palette['table_line']),('BACKGROUND',(0,0),(-1,len(heads)-1),palette['table_header']),('ROWBACKGROUNDS',(0,len(heads)),(-1,-1),[colors.white,palette['table_alt']]),('LEFTPADDING',(0,0),(-1,-1),pdf['table_padding']['horizontal']),('RIGHTPADDING',(0,0),(-1,-1),pdf['table_padding']['horizontal']),('TOPPADDING',(0,0),(-1,-1),pdf['table_padding']['vertical']),('BOTTOMPADDING',(0,0),(-1,-1),pdf['table_padding']['vertical'])]))
                    table.wrap(width,A4[1])
                    table.min_start_height=sum(table._rowHeights[:len(heads)+pdf['pagination']['table_start_rows']])
                    table.spaceBefore=4;table.spaceAfter=8
                    flow.append(table)
                elif kind=='HorizontalRule':flow.append(Spacer(1,6))
                else:raise ValueError('Unsupported Markdown block: '+kind)
            return flow
        records=[]
        class Document(BaseDocTemplate):
            def beforeDocument(self):records.clear()
            def afterFlowable(self,flowable):
                if isinstance(flowable,(Paragraph,LongTable)):
                    text=flowable.getPlainText() if isinstance(flowable,Paragraph) else 'table'
                    records.append(dict(page=self.page,kind='heading' if hasattr(flowable,'section_level') else 'table' if isinstance(flowable,LongTable) else 'body',text=text,bottom=self.frame._y+flowable.getSpaceAfter(),top=self.frame._y+flowable.getSpaceAfter()+(getattr(flowable,'height',0) or getattr(flowable,'_height',0))))
                if hasattr(flowable,'section_level'):
                    level=flowable.section_level;key='section-'+str(self.seq.nextf('section'))
                    self.canv.bookmarkPage(key)
                    self.canv.addOutlineEntry(flowable.section_text,key,level=max(0,level-2),closed=False)
                    if level==2:self.notify('TOCEntry',(0,flowable.section_text,self.page,key))
        doc=Document(str(target),pagesize=A4,leftMargin=margins['left'],rightMargin=margins['right'],topMargin=margins['top'],bottomMargin=margins['bottom'],title=meta['handbook_title']+' '+meta['handbook_version'],author=meta['author'])
        def page(canvas,doc):
            canvas.setStrokeColor(palette['rule']);canvas.setLineWidth(.4)
            canvas.line(margins['left'],A4[1]-30,A4[0]-margins['right'],A4[1]-30)
            canvas.setFont('CJK',8);canvas.setFillColor(muted)
            canvas.drawString(margins['left'],A4[1]-23,labels['header_left'])
            canvas.drawRightString(A4[0]-margins['right'],A4[1]-23,labels['header_right'])
            canvas.drawString(margins['left'],23,labels['footer_left'])
            canvas.drawRightString(A4[0]-margins['right'],23,str(doc.page))
        doc.addPageTemplates(PageTemplate(id='handbook',frames=[Frame(margins['left'],margins['bottom'],width,A4[1]-margins['top']-margins['bottom'],leftPadding=0,rightPadding=0,topPadding=0,bottomPadding=0)],onPage=page))
        toc=TableOfContents();toc.levelStyles=[styles['toc']]
        story=[para(meta['handbook_title'],'title')]+render(metadata)
        if args.preview and pdf['preview_label']:story.append(para(pdf['preview_label'],'small'))
        story+=[Spacer(1,6),para(labels['contents_title'],'h3'),toc,Spacer(1,12)]+render(body)
        from pdf_layout import prepare_flow,audit
        usable=A4[1]-margins['top']-margins['bottom']
        # Keep a short final chapter together by measured size, never by its name.
        starts=[i for i,x in enumerate(story) if getattr(x,'section_level',None)==2]
        if starts:
            tail=story[starts[-1]:];estimated=0
            for x in tail:
                h=x.min_start_height if isinstance(x,KeepTogether) else x.wrap(width,usable)[1]
                estimated+=h+x.getSpaceBefore()+x.getSpaceAfter()
            if estimated<=usable*pdf['pagination']['keep_last_short_chapter_ratio']:
                group=KeepTogether(tail);group.min_start_height=estimated
                story=story[:starts[-1]]+[group]
        story=prepare_flow(story,width,usable,pdf['pagination']['minimum_body_lines'])
        doc.multiBuild(story)
        warnings=audit(records,A4[1],margins['top'],margins['bottom'],pdf['pagination']['sparse_last_page_ratio'])
        if args.layout_report:
            args.layout_report.parent.mkdir(parents=True,exist_ok=True)
            args.layout_report.write_text(json.dumps(dict(records=records,warnings=warnings),ensure_ascii=False,indent=2),encoding='utf-8')
        if any(x['kind'] in ['orphan_heading','out_of_bounds'] for x in warnings):raise ValueError('PDF layout check failed: '+str(warnings))
        if warnings:print('Layout warnings: '+str(warnings))
    print(target)


if __name__=='__main__':main()
