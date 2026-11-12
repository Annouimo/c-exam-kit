/* UTF-8 inside; Unicode Windows console I/O at the boundary.
 * Files and clipboard data are never converted by the display adapter.
 * These wrappers affect stdout/stderr only; ordinary FILE streams stay bytes.
 */
#include <stdarg.h>
#include <limits.h>

static int io_input_ended;
#ifdef _WIN32
static UINT io_redirect_codepage=CP_UTF8;
typedef struct {unsigned char bytes[4];size_t count;} Utf8Tail;
static Utf8Tail io_tails[2];

static int io_console(DWORD which) {
    DWORD mode;
    return GetConsoleMode(GetStdHandle(which),&mode)!=0;
}

/* Retain an incomplete final UTF-8 character across single-byte writes. */
static size_t io_complete_prefix(const unsigned char *s,size_t n) {
    if(!n)return 0;
    size_t lead=n-1;
    while(lead && (s[lead]&0xc0)==0x80)lead--;
    unsigned char ch=s[lead];
    size_t need=ch<0x80?1:ch>=0xc2&&ch<=0xdf?2:ch>=0xe0&&ch<=0xef?3:ch>=0xf0&&ch<=0xf4?4:1;
    return n-lead<need?lead:n;
}

static int io_write_windows(FILE *f,const char *text,size_t length) {
    int slot=f==stderr?1:0;
    DWORD which=slot?STD_ERROR_HANDLE:STD_OUTPUT_HANDLE;
    int console=io_console(which);
    if(!console && io_redirect_codepage==CP_UTF8)
        return fwrite(text,1,length,f)==length;
    Utf8Tail *tail=&io_tails[slot];
    if(length>(size_t)INT_MAX-tail->count)return 0;
    size_t total=tail->count+length;
    char *all=(char *)malloc(total+1);
    if(!all)return 0;
    memcpy(all,tail->bytes,tail->count);memcpy(all+tail->count,text,length);
    size_t complete=io_complete_prefix((const unsigned char *)all,total);
    tail->count=total-complete;
    if(tail->count)memcpy(tail->bytes,all+complete,tail->count);
    if(!complete) {free(all);return 1;}
    int count=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,all,(int)complete,NULL,0);
    if(!count) {free(all);return 0;}
    wchar_t *wide=(wchar_t *)malloc((size_t)count*sizeof *wide);
    if(!wide) {free(all);return 0;}
    int ok=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,all,(int)complete,wide,count)==count;
    free(all);
    if(ok && console) {
        int sent=0;
        while(sent<count) {
            DWORD part=(DWORD)(count-sent),written=0;
            if(part>16384)part=16384;
            /* Do not split a UTF-16 surrogate pair between calls. */
            if(sent+(int)part<count && wide[sent+part-1]>=0xd800 && wide[sent+part-1]<=0xdbff)part--;
            if(!WriteConsoleW(GetStdHandle(which),wide+sent,part,&written,NULL)||!written) {ok=0;break;}
            sent+=(int)written;
        }
    } else if(ok) {
        int bytes=WideCharToMultiByte(io_redirect_codepage,0,wide,count,NULL,0,NULL,NULL);
        char *encoded=bytes?(char *)malloc((size_t)bytes):NULL;
        ok=encoded && WideCharToMultiByte(io_redirect_codepage,0,wide,count,encoded,bytes,NULL,NULL)==bytes;
        if(ok)ok=fwrite(encoded,1,(size_t)bytes,f)==(size_t)bytes;
        free(encoded);
    }
    free(wide);return ok;
}
#endif

static size_t io_fwrite(const void *ptr,size_t size,size_t count,FILE *f) {
    if(!size||!count)return 0;
#ifdef _WIN32
    if(f==stdout||f==stderr) {
        if(size && count>SIZE_MAX/size)return 0;
        return io_write_windows(f,(const char *)ptr,size*count)?count:0;
    }
#endif
    return fwrite(ptr,size,count,f);
}
static int io_fputs(const char *s,FILE *f) {
    size_t n=strlen(s);return io_fwrite(s,1,n,f)==n?0:EOF;
}
static int io_fputc(int ch,FILE *f) {
    unsigned char c=(unsigned char)ch;
    return io_fwrite(&c,1,1,f)==1?c:EOF;
}
static int io_puts(const char *s) {
    return io_fputs(s,stdout)==EOF||io_fputc('\n',stdout)==EOF?EOF:0;
}
static int io_putchar(int ch) {return io_fputc(ch,stdout);}
static int io_vfprintf(FILE *f,const char *format,va_list args) {
    va_list copy;va_copy(copy,args);
    int n=vsnprintf(NULL,0,format,copy);va_end(copy);
    if(n<0)return -1;
    char *text=(char *)malloc((size_t)n+1);
    if(!text)return -1;
    int written=vsnprintf(text,(size_t)n+1,format,args);
    int ok=written==n && io_fwrite(text,1,(size_t)n,f)==(size_t)n;
    free(text);return ok?n:-1;
}
static int io_fprintf(FILE *f,const char *format,...) {
    va_list args;va_start(args,format);int n=io_vfprintf(f,format,args);va_end(args);return n;
}
static int io_printf(const char *format,...) {
    va_list args;va_start(args,format);int n=io_vfprintf(stdout,format,args);va_end(args);return n;
}
static void io_perror(const char *label) {
    int error=errno;
    /* Keep diagnostics in our known encoding; libc messages may use the ACP. */
    io_fprintf(stderr,"%s: error %d\n",label,error);
}

static int io_stdio_encoding(const char *encoding) {
    if(!strcmp(encoding,"utf8")||!strcmp(encoding,"utf-8")) {
#ifdef _WIN32
        io_redirect_codepage=CP_UTF8;
#endif
        return 1;
    }
#ifdef _WIN32
    if(!strcmp(encoding,"gbk")) {io_redirect_codepage=936;return 1;}
#endif
    return 0;
}

/* 1=one line, 0=EOF, -1=too long/invalid. Always consume the whole line. */
static int io_read_line(char *out,size_t cap) {
#ifdef _WIN32
    if(io_console(STD_INPUT_HANDLE)) {
        wchar_t *line=(wchar_t *)malloc(cap*sizeof *line);
        if(!line)return -1;
        size_t used=0;int overflow=0,ended=0;
        for(;;) {
            wchar_t ch;DWORD got=0;
            if(!ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE),&ch,1,&got,NULL)||!got) {ended=1;break;}
            if(ch==L'\n')break;
            if(used+1<cap)line[used++]=ch;else overflow=1;
        }
        if(ended)io_input_ended=1;
        if((ended&&!used)||(used&&line[0]==26)) {io_input_ended=1;free(line);return 0;}
        if(used&&line[used-1]==L'\r')used--;
        int bytes=used?WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,line,(int)used,NULL,0,NULL,NULL):0;
        int ok=!overflow && (!used||bytes>0) && (size_t)bytes<cap;
        if(ok&&used)ok=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,line,(int)used,out,bytes,NULL,NULL)==bytes;
        if(ok)out[bytes]=0;
        free(line);return ok?1:-1;
    }
#endif
    if(!fgets(out,(int)cap,stdin)) {io_input_ended=1;return 0;}
    size_t n=strlen(out);
    if(n&&out[n-1]=='\n')out[--n]=0;
    else if(!feof(stdin)) {
        int ch;while((ch=getchar())!='\n'&&ch!=EOF) {}
        return -1;
    }
    if(n&&out[n-1]=='\r')out[--n]=0;
#ifdef _WIN32
    if(io_redirect_codepage!=CP_UTF8 && n) {
        int count=MultiByteToWideChar(io_redirect_codepage,MB_ERR_INVALID_CHARS,out,(int)n,NULL,0);
        wchar_t *wide=count?(wchar_t *)malloc((size_t)count*sizeof *wide):NULL;
        if(!wide)return -1;
        int ok=MultiByteToWideChar(io_redirect_codepage,MB_ERR_INVALID_CHARS,out,(int)n,wide,count)==count;
        int bytes=ok?WideCharToMultiByte(CP_UTF8,0,wide,count,NULL,0,NULL,NULL):0;
        if(bytes<=0||(size_t)bytes>=cap)ok=0;
        if(ok)ok=WideCharToMultiByte(CP_UTF8,0,wide,count,out,bytes,NULL,NULL)==bytes;
        if(ok)out[bytes]=0;
        free(wide);if(!ok)return -1;
    }
#endif
    return 1;
}

/* Keep formatted display and byte-oriented generation behind the same API.
 * Since wrappers check FILE identity, saved source/manual bytes remain UTF-8.
 */
#define printf io_printf
#define fprintf io_fprintf
#define fputs io_fputs
#define fputc io_fputc
#define puts io_puts
#define putchar io_putchar
#define fwrite io_fwrite
#define perror io_perror
