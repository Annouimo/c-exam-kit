"""Convert C tokens to ReportLab markup without changing code text."""
import html
import re


def highlight_lines(source, palette):
    from pygments.lexers import CLexer
    from pygments.token import Comment, Keyword, Name, Number, String, Operator, Punctuation
    lexer=CLexer()
    lines=['']
    # C lexers need a final newline to recognize a trailing // comment.
    # Clip the synthetic newline so highlighted text remains byte-for-byte equivalent.
    lex_source=source if source.endswith('\n') else source+'\n'
    for offset,token,value in lexer.get_tokens_unprocessed(lex_source):
        value=value[:max(0,len(source)-offset)]
        if not value:continue
        category='text'
        if token in Comment:category='comment'
        elif token in Keyword.Type:category='type'
        elif token in Keyword:category='keyword'
        elif token in String:category='string'
        elif token in Number:category='number'
        elif token in Name:
            if value in {'FILE','size_t','ptrdiff_t','wchar_t'}:category='type'
            elif token in Name.Function or re.match(r'\s*\(',source[offset+len(value):]):category='function'
            elif value.isupper():category='constant'
        elif token in Operator or token in Punctuation:category='operator'
        parts=value.split('\n')
        for i,part in enumerate(parts):
            if i:lines.append('')
            if not part:continue
            text=''.join('<font name="CJK">'+html.escape(run)+'</font>' if any(ord(c)>127 for c in run)
                else html.escape(run).replace(' ','&#160;').replace('\t','&#160;'*4)
                for run in re.findall(r'[^\x00-\x7f]+|[\x00-\x7f]+',part))
            lines[-1]+='<font color="'+palette[category]+'">'+text+'</font>'
    return lines
