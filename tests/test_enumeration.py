#!/usr/bin/env python3
"""Compare the catalog iterators with Python's itertools, including lifecycle edges."""
from pathlib import Path
import itertools
import json
import os
import shutil
import subprocess
import tempfile
import sys

root = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(root / 'tools'))
import build as builder
pieces = {p['id']: p for p in builder.load_catalog()['pieces']}
code = '#include <stdio.h>\n#include <stddef.h>\n#define ENUM_MAX 64\n' + '\n'.join(pieces[k]['code'] for k in ['arrangement_iterator', 'combination_iterator'])
cc = os.environ.get('CC') or shutil.which('cc') or shutil.which('gcc')
assert cc, 'A C compiler is required'
with tempfile.TemporaryDirectory() as tmp:
    folder = Path(tmp)
    source = folder / 'test.c'
    source.write_text(code + '\n#include <assert.h>\n' + r'''
int main(void) {
    for (int mode = 0; mode < 2; ++mode) {
        for (int n = 0; n <= 7; ++n) for (int r = 0; r <= n; ++r) {
            Arrangement a = arrangement(n, r);
            Combination c = combination(n, r);
            assert(a.valid && c.valid);
            while (mode ? combination_next(&c) : arrangement_next(&a)) {
                const int *p = mode ? c.index : a.index;
                printf("%d %d %d:", mode, n, r);
                for (int j = 0; j < r; ++j) printf(" %d", p[j]);
                puts("");
            }
            if (mode) assert(!combination_next(&c));
            else assert(!arrangement_next(&a));
        }
    }
    int invalid[][2] = {{-1,0},{1,-1},{2,3},{ENUM_MAX+1,1}};
    for (int i = 0; i < 4; ++i) {
        Arrangement a = arrangement(invalid[i][0], invalid[i][1]);
        Combination c = combination(invalid[i][0], invalid[i][1]);
        assert(!a.valid && !c.valid);
        assert(!arrangement_next(&a) && !combination_next(&c));
    }
    assert(!arrangement_next(NULL) && !combination_next(NULL));
    Arrangement a = arrangement(3,2);
    assert(arrangement_next(&a));
    Arrangement b = a;
    assert(arrangement_next(&a) && b.index[0] == 0 && b.index[1] == 1);
    assert(arrangement_next(&b) && b.index[1] == 2);
    a = arrangement(3,2);
    assert(arrangement_next(&a) && a.index[1] == 1);
    Combination c = combination(3,2);
    assert(combination_next(&c));
    Combination d = c;
    assert(combination_next(&c) && d.index[1] == 1);
    assert(combination_next(&d) && d.index[1] == 2);
    c = combination(3,2);
    assert(combination_next(&c) && c.index[1] == 1);
    a = arrangement(ENUM_MAX,1);
    c = combination(ENUM_MAX,ENUM_MAX);
    assert(a.valid && c.valid && combination_next(&c));
    assert(c.index[ENUM_MAX-1] == ENUM_MAX-1 && !combination_next(&c));
    int count = 0;
    while (arrangement_next(&a)) assert(a.index[0] == count++);
    assert(count == ENUM_MAX);
    return 0;
}
''', encoding='utf-8')
    exe = folder / 'test.exe'
    subprocess.run([cc, '-std=c99', '-Wall', '-Wextra', '-Werror', str(source), '-o', str(exe)], check=True)
    lines = subprocess.check_output([str(exe)], text=True).splitlines()
    expected = []
    for mode, func in enumerate((itertools.permutations, itertools.combinations)):
        for n in range(8):
            for r in range(n + 1):
                expected.extend(f'{mode} {n} {r}:' + ''.join(f' {i}' for i in p)
                                for p in func(range(n), r))
    assert lines == expected, 'Iterator output differs from itertools'
    print(f'OK: {len(lines)} results match itertools; invalid inputs, empty results, capacity, exhaustion, copies and restart passed')
