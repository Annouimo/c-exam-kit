"""Flow-based pagination helpers; no chapter-specific page breaks."""


def prepare_flow(flow, width, height, minimum_lines=2):
    from reportlab.platypus import CondPageBreak, Paragraph, Spacer
    result=[]
    for i,item in enumerate(flow):
        if getattr(item,'section_level',None):
            required=0.0
            for following in flow[i:]:
                known=getattr(following,'min_start_height',None)
                h=known if known is not None else following.wrap(width,height)[1]
                if getattr(following,'section_level',None) or isinstance(following,Spacer):
                    required+=h+following.getSpaceBefore()+following.getSpaceAfter()
                    continue
                start=getattr(following,'min_start_height',None)
                if start is None:
                    start=min(h,following.style.leading*minimum_lines) if isinstance(following,Paragraph) else h
                required+=start+following.getSpaceBefore()
                break
            result.append(CondPageBreak(min(required,height)))
        result.append(item)
    return result


def audit(records,page_height,top_margin,bottom_margin,last_page_ratio=.15):
    warnings=[]
    for i,row in enumerate(records):
        if row['kind']=='heading':
            following=next((x for x in records[i+1:] if x['kind']!='heading'),None)
            if following is None or following['page']!=row['page']:
                warnings.append({'kind':'orphan_heading','page':row['page'],'text':row['text']})
        if row['bottom']<bottom_margin-.5 or row['top']>page_height-top_margin+.5:
            warnings.append({'kind':'out_of_bounds','page':row['page'],'text':row['text']})
    if records:
        last=max(x['page'] for x in records)
        rows=[x for x in records if x['page']==last]
        occupied=max(x['top'] for x in rows)-min(x['bottom'] for x in rows)
        if last>1 and occupied/(page_height-top_margin-bottom_margin)<last_page_ratio:
            warnings.append({'kind':'sparse_last_page','page':last,'ratio':occupied/(page_height-top_margin-bottom_margin)})
    return warnings
