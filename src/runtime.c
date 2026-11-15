/* C语言脚手架生成器：请先编译运行，再按菜单选择组件。
 * C Scaffold Generator: compile and run this program before selecting components.
 * 分发版由构建脚本生成ASCII单文件；中文开发源码保留在src/与templates/。
 */
#define COUNT ((int)(sizeof components/sizeof components[0]))
#define PIECES ((int)(sizeof pieces/sizeof pieces[0]))
#define GROUPS ((int)(sizeof group_names/sizeof group_names[0]))
#define FRAGMENTS ((int)(sizeof fragments/sizeof fragments[0]))
#define IO_COUNT ((int)(sizeof io_names/sizeof io_names[0]))
#define IO_GROUPS ((int)(sizeof io_group_names/sizeof io_group_names[0]))
#define FRAGMENT_GROUPS ((int)(sizeof fragment_group_names/sizeof fragment_group_names[0]))
/* @CONFIG@ */
static int interactive, clear_enabled, banner_shown;

/* Color is optional; readable headings remain in plain/redirected output. */
static void emphasis(FILE *f,const char *tag,const char *message,int level) {
    const char *colors[]={"\033[1;36m","\033[1;33m","\033[1;32m","\033[1;31m"};
    int color=f==stdout && clear_enabled;
    if(color)fputs(colors[level>=0&&level<=3?level:0],f);
    fprintf(f,"【%s】 %s\n",tag,message);
    if(color)fputs("\033[0m",f);
}
static int *setting_value(Config *c,const Setting *s) {
    return (int *)((char *)c+s->offset);
}
static int setting_read(const Config *c,const Setting *s) {
    return *(const int *)((const char *)c+s->offset);
}
static Config defaults(void) {
    Config c; memset(&c,0,sizeof c); c.io=DEFAULT_IO;
    strcpy(c.separator," ");c.comments=1;c.flow=1;
    for(int i=0;i<SETTINGS;i++)*setting_value(&c,&settings[i])=settings[i].def;
    return c;
}
static int read_line(char *s, size_t cap) {
    int result=io_read_line(s,cap);
    if(result<0)emphasis(stdout,"输入无效","内容过长或编码无效，请检查后重新输入。",1);
    return result;
}
static int ask(const char *label, int def, int low, int high) {
    char s[128],*end;
    for(;;) {
        printf("%s [%d]（回车采用；q取消）：",label,def); fflush(stdout);
        int ok=read_line(s,sizeof s);
        if(!ok) return -1;
        if(ok<0) continue;
        if(!strcmp(s,"q"))return -1;
        if(!*s) return def;
        errno=0; long n=strtol(s,&end,10);
        while(*end==' ' || *end=='\t') end++;
        if(!errno && end!=s && !*end && (n==-1 || (n>=low&&n<=high))) return (int)n;
        char message[128]; snprintf(message,sizeof message,"请输入%d..%d之间的整数。",low,high); emphasis(stdout,"输入有误",message,1);
    }
}
static void terminal_init(int plain) {
#ifdef _WIN32
    interactive=io_console(STD_INPUT_HANDLE)&&io_console(STD_OUTPUT_HANDLE);
    if(interactive && !plain) {
        DWORD mode; HANDLE h=GetStdHandle(STD_OUTPUT_HANDLE);
        clear_enabled=GetConsoleMode(h,&mode) && SetConsoleMode(h,mode|0x0004);
    }
#else
    interactive=isatty(0)&&isatty(1);
    const char *term=getenv("TERM");
    clear_enabled=interactive&&!plain&&term&&strcmp(term,"dumb");
#endif
    (void)plain;
}
static int terminal_width(void) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    if(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&info))
        return info.srWindow.Right-info.srWindow.Left+1;
#else
    struct winsize size;
    if(ioctl(1,TIOCGWINSZ,&size)==0 && size.ws_col) return size.ws_col;
#endif
    return 76;
}
/* UTF-8 display width for this menu's ASCII and CJK labels. No byte-count padding. */
static int display_width(const char *s) {
    int width=0;
    while(*s) {
        unsigned char ch=(unsigned char)*s++;
        if(ch<0x80) width++;
        else {
            unsigned code=0; int extra=0;
            if(ch>=0xc0&&ch<0xe0) {code=ch&31;extra=1;}
            else if(ch>=0xe0&&ch<0xf0) {code=ch&15;extra=2;}
            else if(ch>=0xf0) {code=ch&7;extra=3;}
            for(int i=0;i<extra && *s;i++) code=(code<<6)|((unsigned char)*s++&63);
            width += (code>=0x1100 && (code<=0x115f || code>=0x2e80)) ? 2 : 1;
        }
    }
    return width;
}
static void screen(int step,const char *title) {
    if(clear_enabled) fputs("\033[2J\033[H",stdout);
    if(clear_enabled || !banner_shown) {
        printf("C语言脚手架 v%s | 设计者：%s\n",APP_VERSION,APP_DESIGNER);
        banner_shown=1;
    }
    printf("[");
    for(int i=1;i<=4;i++) fputs(i<=step?"##":"--",stdout);
    printf("] %d/4 %s\n",step,title);
}
/* Fill each category block with short labels, wrapping at the actual column width. */
typedef struct {char text[80][256];int length;} MenuBlock;
static MenuBlock make_block(int group,int width) {
    MenuBlock b; memset(&b,0,sizeof b);
    snprintf(b.text[b.length++],256,"【%s】",group_names[group]);
    char line[256]="";
    for(int i=0;i<COUNT;i++) {
        if(components[i].group!=group) continue;
        char token[128];snprintf(token,sizeof token,"%2d %s",i+1,components[i].short_name);
        int need=display_width(line)+(*line?3:0)+display_width(token);
        if(*line && need>width) {strcpy(b.text[b.length++],line);line[0]=0;}
        if(*line) strcat(line,"   ");
        strcat(line,token);
    }
    if(*line) strcpy(b.text[b.length++],line);
    return b;
}
static void menu_text(const char *text) {
    if(clear_enabled && !strncmp(text,"【",strlen("【"))) {
        fputs("\033[1;36m",stdout);fputs(text,stdout);fputs("\033[0m",stdout);
    } else fputs(text,stdout);
}
static void menu_components(void) {
    int width=terminal_width();
    if(width>=96) {
        int col=(width-4)/2;
        if(col>100) col=100;
        MenuBlock left,right;memset(&left,0,sizeof left);memset(&right,0,sizeof right);
        /* Blocks stay intact; split by content height to use the shorter viewport. */
        MenuBlock blocks[GROUPS];int total=0;
        for(int g=0;g<GROUPS;g++) {blocks[g]=make_block(g,col);total+=blocks[g].length+1;}
        int acc=0,split=0;
        while(split<GROUPS && acc<total/2) acc+=blocks[split++].length+1;
        for(int g=0;g<GROUPS;g++) {
            MenuBlock *out=g<split?&left:&right;
            if(out->length) out->text[out->length++][0]=0;
            for(int k=0;k<blocks[g].length;k++) strcpy(out->text[out->length++],blocks[g].text[k]);
        }
        int rows=left.length>right.length?left.length:right.length;
        for(int row=0;row<rows;row++) {
            const char *l=row<left.length?left.text[row]:"";
            const char *r=row<right.length?right.text[row]:"";
            menu_text(l);
            for(int k=display_width(l);k<col+4;k++) putchar(' ');
            menu_text(r);putchar('\n');
        }
    } else {
        for(int g=0;g<GROUPS;g++) {
            MenuBlock b=make_block(g,width>12?width-2:12);
            for(int i=0;i<b.length;i++) {menu_text(b.text[i]);putchar('\n');}
        }
    }
}
static int any_selected(const Config *c) {
    for(int i=0;i<COUNT;i++) if(c->selected[i]) return 1;
    return 0;
}
static int choose_components(Config *c) {
    char s[1024];
    const char *message=NULL;
    for(;;) {
        screen(2,io_options[c->io].functions_only?"选择函数组件（至少选一个）":UI_COMPONENT_TITLE);
        printf("已选输入输出：%s\n",io_names[c->io]);
        menu_components();
        puts(UI_COMPONENT_PROMPT);
        if(message) emphasis(stdout,"请重新选择",message,1);
        printf("组件：");fflush(stdout);
        int ok=read_line(s,sizeof s);
        if(!ok || (ok>0&&!strcmp(s,"q"))) return 0;
        if(ok<0) continue;
        if(s[0]=='?') {
            for(int i=0;i<COUNT;i++) if(!s[1] || strstr(components[i].name,s+1) || strstr(components[i].help,s+1))
                printf("%d %s\n",i+1,components[i].name);
            puts("回车返回组件列表。");
            if(!read_line(s,sizeof s)) return 0;
            continue;
        }
        if(!*s) {
            if(io_options[c->io].functions_only&&!any_selected(c)) {message=UI_FUNCTIONS_REQUIRED;continue;}
            return 1;
        }
        int selected[COUNT]={0},valid=1,zero=0,count=0;
        char *p=s;
        while(*p) {
            while(*p==' '||*p=='\t'||*p==',') p++;
            if(!*p) break;
            char *end;errno=0;long n=strtol(p,&end,10);
            if(errno||p==end||n<0||n>COUNT||(*end&&*end!=' '&&*end!='\t'&&*end!=',')) {valid=0;break;}
            if(!n) zero=1; else {selected[n-1]=1;count++;}
            p=end;
        }
        if(!valid || (zero&&count)) {message="编号无效，原选择保留，请重输。";continue;}
        if(io_options[c->io].functions_only&&!count) {message="仅函数模式不能清空，请选择组件或取消。";continue;}
        memcpy(c->selected,selected,sizeof selected);return 1;
    }
}
static int choose_flow(Config *c) {
    screen(1,UI_FLOW_TITLE);
    for(int i=0;i<FLOWS;i++) {
        if(i==0)puts("\n【自己写主体】");
        if(i==2)puts("\n【重复处理数据】");
        if(i==5)puts("\n【分阶段与操作指令】");
        if(i==8)puts("\n【文本文件】");
        printf("%2d %s\n",i+1,flows[i].name);
    }
    printf("\n%d %s\n",FLOWS+1,UI_FLOW_OTHER);
    int pick=ask(UI_FLOW_PROMPT,c->flow+1,1,FLOWS+1);
    if(pick<0)return 0;
    if(pick==FLOWS+1)return 2;
    const Flow *flow=&flows[pick-1];
    int io=flow->io,reader=0;
    if(flow->count) {
        for(int i=0;i<flow->count;i++)printf("%d %s\n",i+1,io_names[flow->choices[i]]);
        int def=1;
        if(c->flow==pick-1)for(int i=0;i<flow->count;i++)if(flow->choices[i]==c->io)def=i+1;
        int chosen=ask(UI_READER_CHOICE,def,1,flow->count);
        if(chosen<0)return 0;
        io=flow->choices[chosen-1];
    }
    if(flow->readers) {
        reader=ask(UI_READER,c->flow==pick-1?c->reader:0,0,2);
        if(reader<0)return 0;
    }
    if(c->flow!=pick-1||c->io!=io||c->reader!=reader)memset(c->snippet,0,sizeof c->snippet);
    c->flow=pick-1;c->io=io;c->reader=reader;
    if(reader)c->snippet[reader==1?SN_READ_ARRAY:SN_READ_MATRIX]=1;
    printf("已选：%s",io_names[io]);
    if(reader)printf("；%s",fragments[reader==1?SN_READ_ARRAY:SN_READ_MATRIX].name);
    putchar('\n');
    return 1;
}
static int choose_extras(Config *c) {
    screen(3,UI_EXTRAS_TITLE);puts(UI_EXTRAS_NOTE);
    int map[FRAGMENTS],count=0;
    for(int i=0;i<FRAGMENTS;i++)if(fragments[i].phase>0) {
        map[count++]=i;printf("%d %s\n",count,fragments[i].name);
    }
    puts(UI_EXTRAS_PROMPT);char line[256];
    for(;;) {
        fputs("片段：",stdout);fflush(stdout);
        int ok=read_line(line,sizeof line);
        if(!ok || (ok>0&&!strcmp(line,"q")))return 0;
        if(ok<0)continue;
        if(!*line)return 1;
        int values[FRAGMENTS]={0},zero=0,chosen=0,bad=0;char *p=line;
        while(*p) {
            while(*p==' '||*p=='\t'||*p==',')p++;
            if(!*p)break;
            char *end;errno=0;long n=strtol(p,&end,10);
            if(errno||p==end||n<0||n>count||(*end&&*end!=' '&&*end!='\t'&&*end!=',')){bad=1;break;}
            if(n) {values[map[n-1]]=1;chosen++;}else zero=1;
            p=end;
        }
        if(bad||(zero&&chosen)){puts("编号无效，原选择保留。");continue;}
        for(int i=0;i<FRAGMENTS;i++)if(fragments[i].phase>0)c->snippet[i]=values[i];
        return 1;
    }
}
static int contains_cap(const char *list,const char *name) {
    size_t n=strlen(name);
    while(*list) {
        const char *end=strchr(list,',');size_t len=end?(size_t)(end-list):strlen(list);
        if(len==n&&!strncmp(list,name,n))return 1;
        if(!end)break;
        list=end+1;
    }
    return 0;
}
static int has_cap(const Config *c,const char *name) {
    if(contains_cap(io_options[c->io].cap,name))return 1;
    for(int i=0;i<COUNT;i++)if(c->selected[i]&&contains_cap(components[i].cap,name))return 1;
    for(int i=0;i<FRAGMENTS;i++)if(c->snippet[i]&&contains_cap(fragments[i].cap,name))return 1;
    return 0;
}
static int needs_value_type(const Config *c) {
    if(io_options[c->io].value_type)return 1;
    for(int i=0;i<COUNT;i++)if(c->selected[i]&&components[i].value_type)return 1;
    for(int i=0;i<FRAGMENTS;i++)if(c->snippet[i]&&fragments[i].value_type)return 1;
    return 0;
}
static int configure_separator(Config *c) {
    if(!c->separator_set) {
        for(int i=0;i<FRAGMENTS;i++)if(c->snippet[i]&&fragments[i].separator_default[0]) {
            strcpy(c->separator,fragments[i].separator_default);break;
        }
    }
    int def=!strcmp(c->separator," ")?1:!strcmp(c->separator,",")?2:!strcmp(c->separator,"+")?3:!*c->separator?5:4;
    int n=ask("元素分隔符：1空格，2逗号，3加号，4自定义字符串，5不分隔",def,1,5);
    if(n<0)return 0;
    if(n==4) {
        char text[sizeof c->separator];
        for(;;) {
            printf("输入分隔字符串（按原样使用；回车保留；q取消）[%s]：",c->separator);fflush(stdout);
            int ok=read_line(text,sizeof text);
            if(!ok||(ok>0&&!strcmp(text,"q")))return 0;
            if(ok<0)continue;
            if(*text)strcpy(c->separator,text);
            break;
        }
    } else strcpy(c->separator,n==1?" ":n==2?",":n==3?"+":"");
    c->separator_set=1;
    emphasis(stdout,"分隔输出","仅在两项之间打印分隔符，末尾换行单独处理。",0);
    return 1;
}
static int configure(Config *c) {
    int needed=needs_value_type(c)||has_cap(c,"SEPARATOR");
    for(int i=0;i<SETTINGS;i++)if(has_cap(c,settings[i].macro))needed=1;
    if(!needed)return 1;
    screen(3,UI_CONFIG_TITLE);
    if(needs_value_type(c)) {int n=ask("选中代码的整数类型：1 int（约±21亿），2 long long（更大）",c->wide+1,1,2);if(n<0)return 0;c->wide=n-1;}
    for(int i=0;i<SETTINGS;i++) {
        const Setting *s=&settings[i];
        if(!has_cap(c,s->macro))continue;
        int n=ask(s->label,setting_read(c,s)-s->extra,s->low,s->high);
        if(n<0)return 0;
        *setting_value(c,s)=n+s->extra;
    }
    if(has_cap(c,"SEPARATOR")&&!configure_separator(c))return 0;
    return 1;
}
/* Produce a safe C string literal; percent signs stay data, never printf formats. */
static void string_literal(const char *s,char out[1024]) {
    char *p=out;*p++='"';
    while(*s) {
        unsigned char ch=(unsigned char)*s++;
        if(ch=='"'||ch=='\\'||ch=='?') {*p++='\\';*p++=(char)ch;}
        else if(ch<32||ch==127) {snprintf(p,5,"\\%03o",ch);p+=4;}
        else *p++=(char)ch;
    }
    *p++='"';*p=0;
}
/* Assemble generated source in memory; no temporary file or writable temp directory. */
typedef struct {
    char *text;
    size_t length, capacity;
    int failed;
} CodeBuffer;
static int buf_reserve(CodeBuffer *b,size_t additional) {
    if(b->failed)return 0;
    if(additional>(size_t)-1-b->length-1) {b->failed=1;return 0;}
    size_t need=b->length+additional+1;
    if(need<=b->capacity)return 1;
    size_t cap=b->capacity?b->capacity:4096;
    while(cap<need) {
        if(cap>((size_t)-1)/2) {cap=need;break;}
        cap*=2;
    }
    char *p=(char *)realloc(b->text,cap);
    if(!p) {b->failed=1;return 0;}
    b->text=p;b->capacity=cap;
    return 1;
}
static void buf_write(CodeBuffer *b,const char *s,size_t n) {
    if(!buf_reserve(b,n))return;
    if(n)memcpy(b->text+b->length,s,n);
    b->length+=n;
    b->text[b->length]=0;
}
static void buf_puts(const char *s,CodeBuffer *b) {
    buf_write(b,s,strlen(s));
}
static void buf_putc(int c,CodeBuffer *b) {
    char ch=(char)c;
    buf_write(b,&ch,1);
}
static void buf_printf(CodeBuffer *b,const char *fmt,...) {
    if(b->failed)return;
    va_list args,copy;
    va_start(args,fmt);
    va_copy(copy,args);
    int size=vsnprintf(NULL,0,fmt,copy);
    va_end(copy);
    if(size<0 || !buf_reserve(b,(size_t)size)) {
        b->failed=1;
        va_end(args);
        return;
    }
    vsnprintf(b->text+b->length,b->capacity-b->length,fmt,args);
    b->length+=(size_t)size;
    va_end(args);
}

static const char *fragment_code(const Config *c,int i) {
    return c->comments==0?plain_fragment_codes[i]:c->comments==1?brief_fragment_codes[i]:fragment_codes[i];
}
static void expand(CodeBuffer *f,const char *s,const Config *c,int indent) {
    int start=1;
    char literal[1024];string_literal(c->separator,literal);
    const char *keys[]={"SEPARATOR_LITERAL","VALUE_SCAN","VALUE_PRINT","VALUE"};
    const char *values[]={literal,c->wide?"\"%lld\"":"\"%d\"",c->wide?"\"%lld\"":"\"%d\"",c->wide?"long long":"int"};
    while(*s) {
        if(start && *s!='\n') {for(int i=0;i<indent;i++)buf_putc(' ',f);start=0;}
        int found=0;
        for(int i=0;i<4;i++) {
            size_t n=strlen(keys[i]);
            if(!strncmp(s,keys[i],n) && !(s[n]>='A'&&s[n]<='Z') && s[n]!='_') {
                buf_puts(values[i],f);s+=n;found=1;break;
            }
        }
        if(!found) {char ch=*s++;buf_putc(ch,f);if(ch=='\n')start=1;}
    }
}

static void emit_inputs(CodeBuffer *f,const Config *c,int indent) {
    for(int i=0;i<FRAGMENTS;i++) if(c->snippet[i] && fragments[i].phase==0)
        expand(f,fragment_code(c,i),c,indent);
}
static void emit_outputs(CodeBuffer *f,const Config *c,int indent) {
    for(int i=0;i<FRAGMENTS;i++) if(c->snippet[i] && fragments[i].phase>0)
        expand(f,fragment_code(c,i),c,indent);
}
static void emit_body(CodeBuffer *f,const Config *c,int indent) {
    emit_inputs(f,c,indent);
    if(c->comments)expand(f,UI_TODO,c,indent);
    emit_outputs(f,c,indent);
}
static void emit_main(CodeBuffer *f,const Config *c) {
    const char *p=c->comments==0?plain_io_codes[c->io]:c->comments==1?brief_io_codes[c->io]:io_codes[c->io];
    while(*p) {
        if(!strncmp(p,"@BODY@",6)) {emit_body(f,c,4);p+=6;if(*p=='\n')p++;}
        else if(!strncmp(p,"@LOOP@",6)) {emit_body(f,c,8);p+=6;if(*p=='\n')p++;}
        else if(!strncmp(p,"@INPUT@",7)) {emit_inputs(f,c,4);p+=7;if(*p=='\n')p++;}
        else if(!strncmp(p,"@OUTPUT@",8)) {emit_outputs(f,c,4);p+=8;if(*p=='\n')p++;}
        else {
            const char *end=strchr(p,'\n');size_t n=end?(size_t)(end-p)+1:strlen(p);
            char line[1024];
            if(n>=sizeof line)n=sizeof line-1;
            memcpy(line,p,n);line[n]=0;expand(f,line,c,0);p+=n;
        }
    }
}
static void emit_piece(CodeBuffer *f,int id,const Config *c) {
    const char *s=c->comments==0?plain_pieces[id]:c->comments==1?brief_pieces[id]:pieces[id];
    const char *wide=c->comments==0?plain_wide_pieces[id]:c->comments==1?brief_wide_pieces[id]:wide_pieces[id];
    if(c->wide&&wide)s=wide;
    while(*s) {
        const char *e=strchr(s,'\n');size_t n=e?(size_t)(e-s)+1:strlen(s);
        if(strncmp(s,"#define ",8)) buf_write(f,s,n);
        s+=n;
    }
}
static void emit(CodeBuffer *f,const Config *c) {
    if(c->comments)buf_puts(code_credit,f);
    int mask=io_options[c->io].headers;
    for(int i=0;i<COUNT;i++)if(c->selected[i])
        for(int j=0;components[i].ids[j]>=0;j++)mask|=piece_headers[components[i].ids[j]];
    for(int i=0;i<FRAGMENTS;i++)if(c->snippet[i])mask|=fragments[i].headers;
    for(int i=0;i<(int)(sizeof header_names/sizeof header_names[0]);i++)
        if(mask&(1<<i))buf_printf(f,"#include <%s>\n",header_names[i]);
    if(mask)buf_putc('\n',f);
    if(c->comments&&io_options[c->io].functions_only)buf_puts(UI_FUNCTION_WARNING,f);
    for(int i=0;i<SETTINGS;i++) {
        const Setting *s=&settings[i];
        if(!has_cap(c,s->macro))continue;
        int value=setting_read(c,s);
        buf_printf(f,"#define %s %d\n",s->macro,value);
        if(s->log_macro[0]) {
            int levels=0;for(int n=value;n;n/=2)levels++;
            buf_printf(f,"#define %s %d\n",s->log_macro,levels);
        }
    }
    if(f->length&&f->text[f->length-1]=='\n'&&(f->length<2||f->text[f->length-2]!='\n'))buf_putc('\n',f);
    int emitted[PIECES]={0};
    for(int i=0;i<COUNT;i++)if(c->selected[i]) {
        if(c->comments)buf_printf(f,UI_CODE_COMPONENT,components[i].name,components[i].note);
        for(int j=0;components[i].ids[j]>=0;j++) {
            int id=components[i].ids[j];
            if(emitted[id])continue;
            /* Each source function carries a short usage note; full details stay in console/manual. */
            if(c->comments==2&&piece_hints[id][0])buf_printf(f,UI_CODE_HINT,piece_hints[id]);
            emit_piece(f,id,c);buf_putc('\n',f);emitted[id]=1;
        }
    }
    if(!io_options[c->io].functions_only) {if(c->comments==2)buf_puts(UI_CODE_MAIN,f);emit_main(f,c);}
}
static char *generate(const Config *c,size_t *size) {
    CodeBuffer output={0};
    emit(&output,c);
    if(output.failed || !output.text) {
        free(output.text);
        return NULL;
    }
    while(output.length>1 && output.text[output.length-1]=='\n' && output.text[output.length-2]=='\n')output.length--;
    output.text[output.length]=0;
    *size=output.length;
    return output.text;
}
/* The manual shares the same in-memory emitter, then writes to its chosen file. */
static void emit_to_file(FILE *f,const Config *c,int part,const char *snippet) {
    CodeBuffer output={0};
    if(part==0)emit_main(&output,c);
    else if(part==1)emit(&output,c);
    else expand(&output,snippet,c,0);
    if(!output.failed && output.text)
        fwrite(output.text,1,output.length,f);
    free(output.text);
}
static int clipboard_copy(const char *s,size_t n) {
#ifdef _WIN32
    /* CF_UNICODETEXT requires UTF-16 and Windows line endings. */
    size_t extra=0;
    for(size_t i=0;i<n;i++) if(s[i]=='\n'&&(i==0||s[i-1]!='\r'))extra++;
    char *crlf=(char *)malloc(n+extra+1);if(!crlf)return 0;
    size_t pos=0;
    for(size_t i=0;i<n;i++) {if(s[i]=='\n'&&(i==0||s[i-1]!='\r'))crlf[pos++]='\r';crlf[pos++]=s[i];}
    crlf[pos]=0;
    int count=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,crlf,-1,NULL,0);
    if(!count) {free(crlf);return 0;}
    HGLOBAL memory=GlobalAlloc(GMEM_MOVEABLE,(SIZE_T)count*sizeof(wchar_t));
    if(!memory) {free(crlf);return 0;}
    wchar_t *dest=(wchar_t *)GlobalLock(memory);
    if(!dest) {free(crlf);GlobalFree(memory);return 0;}
    int converted=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,crlf,-1,dest,count);
    GlobalUnlock(memory);free(crlf);
    if(!converted) {GlobalFree(memory);return 0;}
    /* EmptyClipboard needs a non-NULL owner for SetClipboardData to succeed. */
    HWND owner=CreateWindowExW(0,L"STATIC",L"",0,0,0,0,0,NULL,NULL,GetModuleHandleW(NULL),NULL);
    if(!owner) {GlobalFree(memory);return 0;}
    if(!OpenClipboard(owner)) {DestroyWindow(owner);GlobalFree(memory);return 0;}
    int ok=EmptyClipboard() && SetClipboardData(CF_UNICODETEXT,memory)!=NULL;
    CloseClipboard();DestroyWindow(owner);if(!ok)GlobalFree(memory);
    return ok;
#elif defined(__APPLE__)
    /* No shell interpolation or temporary source file; pipe bytes directly to pbcopy. */
    int fd[2];if(pipe(fd)!=0)return 0;
    pid_t child=fork();
    if(child<0){close(fd[0]);close(fd[1]);return 0;}
    if(child==0) {
        close(fd[1]);if(dup2(fd[0],STDIN_FILENO)<0)_exit(127);close(fd[0]);
        execl("/usr/bin/pbcopy","pbcopy",(char *)NULL);_exit(127);
    }
    close(fd[0]);
    void (*old_signal)(int)=signal(SIGPIPE,SIG_IGN);
    size_t sent=0;int ok=1;
    while(sent<n) {
        ssize_t part=write(fd[1],s+sent,n-sent);
        if(part<0 && errno==EINTR)continue;
        if(part<=0) {ok=0;break;}sent+=(size_t)part;
    }
    close(fd[1]);signal(SIGPIPE,old_signal);
    int status=0;pid_t waited;
    do {waited=waitpid(child,&status,0);} while(waited<0&&errno==EINTR);
    return ok && waited==child && WIFEXITED(status)&&WEXITSTATUS(status)==0;
#else
    (void)s;(void)n;return 0;
#endif
}
static FILE *open_file(const char *path,const char *mode) {
#ifdef _WIN32
    int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,NULL,0);
    if(!n) return NULL;
    wchar_t *wide=(wchar_t *)malloc((size_t)n*sizeof(wchar_t));
    if(!wide) return NULL;
    if(!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,wide,n)) {free(wide);return NULL;}
    wchar_t wide_mode[8];int i=0;
    for(;mode[i] && i<7;i++)wide_mode[i]=(wchar_t)mode[i];
    wide_mode[i]=0;
    FILE *f=_wfopen(wide,wide_mode);free(wide);return f;
#else
    return fopen(path,mode);
#endif
}
static FILE *open_output(const char *def,char path[1024]) {
    for(;;) {
        printf("保存路径 [回车=%s；q取消]：",def);fflush(stdout);
        int ok=read_line(path,1024);
        if(!ok||(ok>0&&!strcmp(path,"-1")))return NULL;
        if(ok<0)continue;
        if(!*path)strcpy(path,def);
        FILE *probe=open_file(path,"r");
        if(probe) {fclose(probe);int choice=ask("已存在：1覆盖，0换路径",0,0,1);if(choice<0)return NULL;if(!choice)continue;}
        FILE *f=open_file(path,"wb");if(f)return f;
        perror("无法保存");
    }
}
static int close_output(FILE *f) {int bad=ferror(f);if(fclose(f)!=0)bad=1;return bad;}
static int save_text(const char *s,size_t n) {
    char path[1024];FILE *f=open_output("scaffold.c",path);if(!f)return 0;
    int bad=fwrite(s,1,n,f)!=n;bad=close_output(f)||bad;
    if(bad) {emphasis(stdout,"保存失败","文件没有完整写入，请换路径或再次保存。",3);return 0;}
    emphasis(stdout,"代码已保存",path,2);return 1;
}
static void explain(FILE *f,const Config *c) {
    if(!any_selected(c)) {
        if(c->io==IO_BLANK&&!c->reader)fprintf(f,"\n%s\n",UI_BLANK_HELP);
        else fprintf(f,"\n%s\n",io_help[c->io]);
    } else {
        fprintf(f,"\n本次函数用法\n");int used[PIECES]={0};
        for(int i=0;i<COUNT;i++)if(c->selected[i]) {
            fprintf(f,"\n%d. %s\n使用前提：%s\n",i+1,components[i].name,components[i].note);
            for(int j=0;components[i].ids[j]>=0;j++) {
                int id=components[i].ids[j];
                if(!used[id])fprintf(f,"%s\n",c->wide&&wide_piece_details[id]?wide_piece_details[id]:piece_details[id]);
                used[id]=1;
            }
        }
        fprintf(f,"\n%s\n",io_options[c->io].functions_only?UI_ONLY_FUNCTION_HELP:UI_GENERAL_HELP);
    }
    for(int i=0;i<FRAGMENTS;i++)if(c->snippet[i])fprintf(f,"%s：%s\n",fragments[i].name,fragments[i].help);
    if(io_options[c->io].incomplete)emphasis(f,"须补齐读法","填写格式、变量地址和成功读取字段数后才能编译。",1);
    if(io_options[c->io].file_write)emphasis(f,"注意","w模式会覆盖同名文件，请先修改文件名。",1);
}
static void handbook(FILE *f) {
    fprintf(f,"# C语言全量脚手架手册 v%s\n\n设计者：%s\n\n先选骨架与组件，再配置。默认复制源码；输入c查看源码、s另存、回车退出。代码需自行完成题意和函数调用。\n\n## 输入输出骨架\n\n",APP_VERSION,APP_DESIGNER);
    for(int i=0;i<IO_COUNT;i++) {
        Config c=defaults();c.io=i;c.comments=2;
        if(i==0 || io_group[i]!=io_group[i-1])fprintf(f,"### %s\n\n",io_group_names[io_group[i]]);
        fprintf(f,"#### %d. %s\n\n%s\n\n",i+1,io_names[i],io_help[i]);
        if(io_options[i].functions_only){fputs("只输出函数、结构体、宏和依赖，接入已有程序前检查重名。\n\n",f);continue;}
        fputs("```c\n",f);
        for(int j=0;j<SETTINGS;j++)if(has_cap(&c,settings[j].macro))fprintf(f,"#define %s %d\n",settings[j].macro,settings[j].def);
        emit_to_file(f,&c,0,NULL);fputs("```\n\n",f);
    }
    fputs("## 组件索引\n\n",f);
    for(int g=0;g<GROUPS;g++) {
        fprintf(f,"### %s\n\n",group_names[g]);
        for(int i=0;i<COUNT;i++)if(components[i].group==g)fprintf(f,"- **%d.** %s：%s\n",i+1,components[i].name,components[i].note);
        fputc('\n',f);
    }
    fputs("## 函数组件详细说明与代码\n\n各组件代码可独立复制；手工合并请去掉共同定义。默认容量为示例，不输出题目专用提示。\n\n",f);
    for(int g=0;g<GROUPS;g++) {
        fprintf(f,"### %s\n\n",group_names[g]);
        for(int i=0;i<COUNT;i++)if(components[i].group==g) {
            Config c=defaults();c.io=IO_FUNCTIONS;c.selected[i]=1;c.comments=2;
            fprintf(f,"#### %d. %s\n\n",i+1,components[i].name);explain(f,&c);
            fputs("\n```c\n",f);emit_to_file(f,&c,1,NULL);fputs("```\n\n",f);
        }
    }
    fputs("## 可选main内片段\n\n片段只提供结构，不自动接线；输出片段放在处理之后；变量名/下标/作用域由题目决定。示例VALUE为int、读入%d，可在程序配置为long long。\n\n",f);
    Config c=defaults();
    for(int i=0;i<FRAGMENTS;i++) {
        if(i==0 || fragments[i].group!=fragments[i-1].group)fprintf(f,"### %s\n\n",fragment_group_names[fragments[i].group]);
        fprintf(f,"#### %d. %s\n\n%s\n\n```c\n",i+1,fragments[i].name,fragments[i].help);
        emit_to_file(f,&c,2,fragments[i].code);fputs("```\n\n",f);
    }
    fputs("## 自测与提交检查\n\n- 多组数据：上一组的计数、访问表或缓存是否残留？\n- 数组：0/1下标、空输入、单元素、重复元素、全负数据。\n- 字符串：空行、末行无换行、前导空格、刚好到容量；fgets长度包含终止符。\n- 数值：类型和scanf格式匹配、乘法先提升类型、位号不超64。\n- 输出：小数位、分隔符、末尾换行、无解文字；不输出生成器提示。\n- 二分：数组有序、整数条件单调且有解、浮点端点连续异号。\n",f);
}
static void after_output(const char *source,size_t length,const Config *c,int delivered) {
    explain(stdout,c);
    if(!delivered)emphasis(stdout,"代码尚未取出",UI_NO_DELIVERY,1);
    if(!interactive||io_input_ended||feof(stdin))return;
    char s[64];
    for(;;) {
        fputs(UI_AFTER_PROMPT,stdout);fflush(stdout);
        int ok=read_line(s,sizeof s);
        if(!ok || (ok>0&&!*s))return;
        if(ok<0)continue;
        if(!strcmp(s,"c")) {puts("===== 源码开始 =====");fwrite(source,1,length,stdout);puts("===== 源码结束 =====");}
        else if(!strcmp(s,"s"))save_text(source,length);
        else emphasis(stdout,"输入有误","请输入c、s或直接回车。",1);
    }
}
static void pause_exit(void) {
    if(!interactive||io_input_ended||feof(stdin))return;
    char s[8];emphasis(stdout,"操作","按回车退出。",0);fflush(stdout);
    while(read_line(s,sizeof s)<0) {}
}
int main(int argc,char **argv) {
    int plain=0,version=0;const char *manual=NULL;
    for(int i=1;i<argc;i++) {
        if(!strcmp(argv[i],"--plain"))plain=1;
        else if(!strcmp(argv[i],"--version"))version=1;
        else if(!strcmp(argv[i],"--manual")&&i+1<argc)manual=argv[++i];
        else if(!strncmp(argv[i],"--stdio-encoding=",17)) {
            if(!io_stdio_encoding(argv[i]+17)) {
                fputs("Unsupported encoding. Use utf8, or gbk on Windows.\n",stderr);return 1;
            }
        } else {
            fputs("Usage: c_scaffold [--plain] [--stdio-encoding=utf8|gbk] [--version | --manual PATH]\n",stderr);return 1;
        }
    }
    if(version&&manual) {fputs("Choose --version or --manual, not both.\n",stderr);return 1;}
    terminal_init(plain);
    if(version) {
        printf("C语言脚手架 v%s | 设计者：%s\n",APP_VERSION,APP_DESIGNER);return 0;
    }
    if(manual) {
        FILE *probe=open_file(manual,"rb");if(probe){fclose(probe);emphasis(stderr,"未保存","文件已存在，请换路径。",1);return 1;}
        FILE *f=open_file(manual,"wb");if(!f){perror(manual);return 1;}handbook(f);return close_output(f);
    }
    Config c=defaults();int stage=1;
    while(1) {
        if(stage==1) {
            int n=choose_flow(&c);
            if(!n){pause_exit();return 0;}
            if(n==2) {
                char path[1024];FILE *f=open_output("C_templates_manual.md",path);
                if(f){handbook(f);if(close_output(f))emphasis(stdout,"保存失败","手册写入失败。",3);else emphasis(stdout,"手册已保存",path,2);}
                pause_exit();return 0;
            }
            stage=2;
        }
        if(stage==2) {
            if(!choose_components(&c)){pause_exit();return 0;}
            stage=3;
        }
        if(stage==3) {
            if(!configure(&c)){pause_exit();return 0;}
            for(;;) {
                screen(3,UI_CONFIRM_TITLE);
                printf("程序：%s\n",io_names[c.io]);
                if(c.reader)printf("读入：%s\n",fragments[c.reader==1?SN_READ_ARRAY:SN_READ_MATRIX].name);
                int count=0;
                for(int i=0;i<COUNT;i++)if(c.selected[i]){printf("函数：%d %s\n",i+1,components[i].name);count++;}
                if(!count)puts("函数：未选择");
                for(int i=0;i<FRAGMENTS;i++)if(c.snippet[i]&&fragments[i].phase>0)printf("补充：%s\n",fragments[i].name);
                printf("注释：%s\n",c.comments==0?"无注释":c.comments==1?"精简（署名、前提、TODO）":"完整");
                if(io_options[c.io].incomplete)puts("注意：本骨架读入表达式需自行补齐。");
                fputs(UI_CONFIRM_PROMPT,stdout);fflush(stdout);
                char line[64];int ok=read_line(line,sizeof line);
                if(!ok||(ok>0&&!strcmp(line,"q"))){pause_exit();return 0;}
                if(ok<0)continue;
                if(!strcmp(line,"2")) {
                    int target=ask(UI_MODIFY_PROMPT,0,0,3);
                    if(target<0){pause_exit();return 0;}
                    if(target){stage=target;break;}
                    continue;
                }
                if(!strcmp(line,"o")) {
                    if(io_options[c.io].functions_only){puts("仅函数模式不添加主体代码。");continue;}
                    Config next=c;if(choose_extras(&next))c=next;
                    if(!configure(&c)){pause_exit();return 0;}continue;
                }
                if(!strcmp(line,"m")) {int mode=ask(UI_COMMENT_PROMPT,c.comments,0,2);if(mode>=0)c.comments=mode;continue;}
                if(*line&&strcmp(line,"1")){puts("请输入上面列出的操作。");continue;}
                int save=*line=='1';size_t length;char *source=generate(&c,&length);
                if(!source){emphasis(stdout,"生成失败","内存不足或无法构建代码。",3);pause_exit();return 1;}
                screen(4,UI_COMPLETE_TITLE);int delivered=0;
                if(save)delivered=save_text(source,length);
                else if(clipboard_copy(source,length)){delivered=1;emphasis(stdout,"复制成功",UI_COPY_OK,2);}
                else {
                    emphasis(stdout,"未复制成功",UI_COPY_FAILED,3);
                    int fallback=ask("补救：1保存文件，2显示代码，0只看说明",1,0,2);
                    if(fallback==1)delivered=save_text(source,length);
                    if(fallback==2){delivered=1;puts("===== 源码开始 =====");fwrite(source,1,length,stdout);puts("===== 源码结束 =====");}
                }
                after_output(source,length,&c,delivered);free(source);return 0;
            }
        }
    }
}
