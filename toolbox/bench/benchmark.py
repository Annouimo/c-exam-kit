#!/usr/bin/env python3
"""Reproducible, exploratory C Toolbox microbenchmarks. No internet needed after vendor checkout."""
from pathlib import Path
import json, os, statistics, subprocess, sys, time

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
OUT = HERE / "_build"
OUT.mkdir(exist_ok=True)
N = int(os.environ.get("BENCH_N", "120000"))
CC = os.environ.get("CC", "gcc")
FLAGS = ["-O2", "-std=c99", "-DNDEBUG", "-Wno-unused-function"]
REPS = 7

def cmd(args, **kwargs):
    return subprocess.run(list(map(str,args)), check=True, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, **kwargs)

def repo(name, url):
    where=OUT/name
    if not where.exists():
        cmd(["git","clone","-q","--depth","1",url,where])
    sha=cmd(["git","-C",where,"rev-parse","HEAD"]).stdout.strip()
    return where,sha

def do_bench(name, body, includes=(), defs=(), incpaths=(), extra=()):
    source=OUT/(name+".c")
    source.write_text("\n".join(list(includes)+list(defs)+[body]),encoding="utf-8")
    executable=OUT/name
    command=[CC,*FLAGS,*(f"-I{x}" for x in incpaths),str(source),"-o",str(executable),*extra]
    t0=time.perf_counter()
    p=subprocess.run(command, text=True, capture_output=True)
    comp=time.perf_counter()-t0
    if p.returncode: return {"name":name,"error":p.stderr[-3500:],"compile_command":command}
    times=[]
    checks=[]
    for _ in range(REPS):
        t0=time.perf_counter()
        p=subprocess.run([str(executable),str(N * 10 if name.startswith('vector_') else N)],capture_output=True,text=True)
        elapsed=time.perf_counter()-t0
        if p.returncode: return {"name":name,"error":"run failed: "+p.stderr[-1000:]}
        times.append(elapsed);checks.append(p.stdout.strip())
    if len(set(checks)) != 1: return {"name":name,"error":"inconsistent checksums"}
    return {"name":name,"compile_sec":round(comp,4),"run_median_sec":round(statistics.median(times),4),"run_range_sec":[round(min(times),4),round(max(times),4)],"check":checks[0],"own_source_bytes":source.stat().st_size,"binary_bytes":executable.stat().st_size}

PRELUDE=String = """
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
static uint32_t rndstate=1234567;
static uint32_t rnd(void){uint32_t x=rndstate;x^=x<<13;x^=x>>17;x^=x<<5;return rndstate=x;}
static void make_key(char *p,uint32_t v){sprintf(p,"key_%08x",v);}
"""
def mk_vec(start, push, get, pop, free):
    return PRELUDE+f"""
int main(int argc,char**argv){{
 int n=atoi(argv[1]); uint64_t s=0;
 {start}
 for(int i=0;i<n;i++){{int x=(int)(rnd()%1000000);{push}}}
 for(int i=0;i<n;i++){{s+=(unsigned){get};}}
 for(int i=0;i<n;i++){{ {pop} }}
 {free}
 printf("%llu\\n",(unsigned long long)s);
 return 0;
}}
"""
def mk_map(start, put, get, free):
    return PRELUDE+f"""
int main(int argc,char**argv){{
 int n=atoi(argv[1]);uint64_t s=0;
 char (*keys)[24]=(char(*)[24])malloc((size_t)n*24);
 for(int i=0;i<n;i++)make_key(keys[i],rnd()% (unsigned)(n*2));
 {start}
 for(int i=0;i<n;i++){{const char*k=keys[i];{put}}}
 for(int i=0;i<n;i++){{const char*k=keys[i];s+=(unsigned long long)({get});}}
 {free}
 free(keys);printf("%llu\\n",(unsigned long long)s);return 0;
}}
"""
def main():
    versions={}
    try:
        stc,versions["STC"]=repo("STC","https://github.com/stclib/STC.git")
        stb,versions["stb"]=repo("stb","https://github.com/nothings/stb.git")
        klib,versions["klib"]=repo("klib","https://github.com/attractivechaos/klib.git")
    except Exception as e:
        print("FETCH FAILED",repr(e));raise
    # Current toolbox is assembled from checked-in templates.
    import importlib.util
    spec=importlib.util.spec_from_file_location("builder",ROOT/"toolbox"/"build.py")
    builder=importlib.util.module_from_spec(spec);spec.loader.exec_module(builder);builder.main()
    import runpy
    toolbox = runpy.run_path(str(ROOT/"toolbox"/"dist"/"c_toolbox_v0.5.0.py"),run_name="benchmark_not_main")
    output=[]
    def case(name, body, include=(), defines=(), paths=(), extra=()):
        result=do_bench(name,body,include,defines,paths,extra)
        print("BENCH_RESULT "+json.dumps(result,ensure_ascii=False),flush=True)
        output.append(result)
    ownvec=toolbox["generate"](["vector"],specs=["IntVector=int"])
    case("vector_toolbox",mk_vec("IntVector v=IntVector_new();","IntVector_push(&v,x);","v.data[i]","IntVector_pop(&v,NULL);","IntVector_free(&v);"),[ownvec])
    case("vector_stb",mk_vec("int *v=NULL;","arrput(v,x);","v[i]","(void)arrpop(v);","arrfree(v);"),["#define STB_DS_IMPLEMENTATION\n#include \"stb_ds.h\"\n"],paths=[stb])
    case("vector_stc",mk_vec("BenchVec v={0};","BenchVec_push(&v,x);","v.data[i]","(void)BenchVec_pop(&v);","BenchVec_drop(&v);"),
         ['#define T BenchVec, int\n#include "stc/vec.h"'],paths=[stc/"include",stc])
    case("vector_klib",mk_vec("kvec_t(int) v; kv_init(v);","kv_push(int,v,x);","kv_A(v,i)","kv_pop(v);","kv_destroy(v);"),['#include "kvec.h"'],paths=[klib])
    ownmap=toolbox["generate"](["map"])
    case("map_toolbox",mk_map("Map map=map_new();","map_set(&map,k,i);","map_get_or(&map,k,-1)","map_free(&map);"),[ownmap])
    case("map_stb",mk_map("struct {char *key; long long value;} *map=NULL; sh_new_strdup(map);","shput(map,k,i);","shget(map,k)","shfree(map);"),["#define STB_DS_IMPLEMENTATION\n#include \"stb_ds.h\"\n"],paths=[stb])
    # STC generic map, cstr manages keys, comparison, hash, and key ownership.
    case("map_stc",mk_map("BenchMap map={0};","BenchMap_emplace_or_assign(&map,k,i);","(BenchMap_get(&map,k) ? BenchMap_get(&map,k)->second : -1)","BenchMap_drop(&map);"),
         ['#define i_implement\n#include "stc/cstr.h"\n#define T BenchMap, cstr, long long, c_pro_key\n#include "stc/hashmap.h"'],paths=[stc/"include",stc])
    case("map_klib",mk_map("khash_t(str) *map=kh_init(str);int ret;khiter_t it;","it=kh_put(str,map,k,&ret);kh_val(map,it)=i;","(kh_get(str,map,k)==kh_end(map) ? -1 : kh_val(map,kh_get(str,map,k)))","kh_destroy(str,map);"),
         ['#include "khash.h"\nKHASH_MAP_INIT_STR(str,long long)'],paths=[klib])
    result={"versions":versions,"cc":CC,"flags":FLAGS,"n":N,"reps":REPS,"results":output}
    (OUT/"results.json").write_text(json.dumps(result,indent=2),encoding="utf-8")
    print("FULL_REPORT "+json.dumps(result,ensure_ascii=False),flush=True)
    failed=[x for x in output if "error" in x]
    if failed: raise SystemExit("Benchmark cases failed: "+", ".join(x["name"] for x in failed))
if __name__=="__main__":main()
