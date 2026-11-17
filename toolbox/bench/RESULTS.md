# C Toolbox backend comparison — exploratory benchmark (2026-11-17)

This is a **microbenchmark**, not a production performance claim or an endorsement of a backend. Source: `toolbox/bench/benchmark.py`. Run: [GitHub Actions run 44018788807](https://github.com/Annouimo/c-exam-kit/actions/runs/44018788807). Both GCC and Clang jobs succeeded.

## Environment

- GitHub Actions `ubuntu-24.04`, x86-64 shared runner; GCC 13.3, Clang 18.1; `-std=c99 -O2 -DNDEBUG -Wno-unused-function`.
- Seven separate process executions per case; median elapsed wall time including short process startup. GNU `/usr/bin/time` records peak resident-set memory.
- Vector: 1,200,000 pseudorandom `int` values, push, sequential sum, pop; 7 repetitions.
- Map: 120,000 string keys (possible duplicates), insert/update to last index, query all keys; 7 repetitions.
- String: append literal `abc` 600,000 times, query length and final byte; 7 repetitions.
- Every implementation in each workload produced the same checksum.
- Library SHAs are pinned in the script:
  - STC: `7928d53440aa2df51d434d5fed8d314df1fb371f`
  - stb: `9600522de53973336fa033a289858a4354cbb04c`
  - klib: `97a0fcb790b43b9e5da8994f4671021fec036f19`
  - SDS: `5347739b1581fcba74fd5cab1fc21d2aef317d71`

## Observations

Elapsed time in **milliseconds**, median. Lower is better.

| Workload | Implementation | GCC 13.3 | Clang 18.1 | GCC peak RSS (MiB approx.) |
|---|---|---:|---:|---:|
| Vector | Toolbox | 6.1 | 6.4 | 6.10 |
| Vector | stb_ds | 6.3 | 6.4 | 6.04 |
| Vector | STC | 6.2 | 6.1 | 6.10 |
| Vector | Klib | 6.1 | 6.4 | 6.10 |
| Map | Toolbox | 32.3 | 37.2 | 15.32 |
| Map | stb_ds | 23.3 | 22.6 | 10.64 |
| Map | STC | 24.2 | 22.2 | 10.89 |
| Map | Klib | 23.1 | 26.5 | 6.35 |
| String | Toolbox | 7.5 | 2.5 | 3.22 |
| String | STC | 5.2 | 3.4 | 3.23 |
| String | SDS | 8.9 | 5.7 | 3.17 |

Source: raw `BENCH_RESULT` logs and `results.json` artifacts from the cited workflow.

## Limitations: compare workflows, not isolated hash algorithms

- The tested Map **key ownership differs**. Toolbox copies every key with a separate allocation; stb_ds `sh_new_strdup` owns copied strings; STC `cstr` has short-string optimization; Klib `KHASH_MAP_INIT_STR` here **borrows key pointers** from a separate array. Thus Klib's memory and performance numbers do not include ownership costs and are not fair evidence of a faster owned-string map.
- The String test measures **only repeated append**. It does not cover slicing, insert/delete, search, ownership or aliasing.
- Toolbox `string_append` currently delegates to `string_insert`, allocating a temporary copy even for ordinary non-aliasing appends; investigate a safe fast path before switching backend.
- Vector runtime is close to process-startup noise: do not rank implementations based on 0.1–0.3 ms differences.
- The libraries provide **different total feature sets**. The tested Toolbox Vector translation unit includes its additional functional/numeric helpers, unlike the minimal other-vector snippets. Compilation times and source-file byte counts are not a normalized measure of equivalent capability; the short third-party test sources reference external headers which are not included in those byte counts.
- GNU peak RSS includes the process, runtime loader and shared libraries, not only the container.
- Shared-runner scheduling and compiler-specific inlining affect numbers; observed String ordering even reverses between GCC and Clang.
- Missing from this run: randomized deletion/rehash, diverse string lengths, pathological collision patterns, custom struct keys/values, allocation-failure tests, sanitizer tests on third-party adapters, GCC 9/old MinGW, and OJ-style single-file submission size.

## Recommendation

1. **Keep the current public API and user workflow**, regardless of backend choice.
2. **Keep the current Vector** for now: little evidence of a runtime problem; focus on deep copy/destructor ownership semantics and modular template generation.
3. **Optimize and retest current String** before changing its representation. The append fast path is a promising targeted improvement. STC is an instructive alternative; compiler sensitivity prevents declaring a universal winner.
4. **Prioritize Map internals**: both compilers show a substantial latency and RSS gap in the benchmark, but control ownership differences before deciding between STC, stb_ds, or a better in-house hash table. Make keys and values generic only after defining copy/move/destructor semantics.
5. Continue to **keep IO custom** and C99/old-toolchain compatible; preserve third-party copyright notices if embedding MIT/BSD sources.
6. For a production choice, run additional **owned-key equivalent** scenarios, deletion/rehash, integers and structs, and test single-file packed source on GCC 9/MinGW.

To reproduce: `python3 toolbox/bench/benchmark.py` from the repository root with Git and internet access. Override `CC=clang` or `BENCH_N=120000`. Exact third-party commits are pinned. The benchmark is isolated in `toolbox/bench/` and does not alter the Toolbox release artifacts.
