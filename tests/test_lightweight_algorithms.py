"""Cross-platform compile/run tests for standalone C99 algorithm preset."""
from pathlib import Path
import json
import random
import math
import os
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
PRESET = ROOT / "scaffold" / "presets" / "exam-io-c99"
MANIFEST = json.loads((PRESET/"manifest.json").read_text(encoding="utf-8"))
COMPONENTS = {x["id"]: x for x in MANIFEST["components"]}
CC = os.environ.get("CC", "cc")


def header(selected=None):
    if selected is None:
        selected = set(COMPONENTS)
    else:
        expanded = set()
        def select(id):
            if id in expanded:
                return
            for dependency in COMPONENTS[id]["requires"]:
                select(dependency)
            expanded.add(id)
        for id in selected:
            select(id)
        selected = expanded
    lines = ["#ifndef EXAM_H", "#define EXAM_H"]
    lines.extend("#include <{}>".format(h) for h in MANIFEST["systemHeaders"])
    lines.extend((PRESET/c["source"]).read_text(encoding="utf-8")
                 for c in MANIFEST["components"] if c["id"] in selected)
    lines.append("#endif")
    return "\n".join(lines) + "\n"


class Algorithms(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="algorithms-c99-")
        self.addCleanup(self.temp.cleanup)
        self.folder = Path(self.temp.name)

    def run_c(self, source, data="", selected=None, expect_error=False):
        (self.folder/"exam.h").write_text(header(selected), encoding="utf-8")
        (self.folder/"main.c").write_text('#include "exam.h"\n'+source,
                                          encoding="utf-8")
        executable = self.folder/("test.exe" if os.name=="nt" else "test")
        cp = subprocess.run([CC, "-std=c99", "-O2", "-Wall", "-Wextra",
                             "-Werror", str(self.folder/"main.c"),
                             "-o", str(executable), "-lm"],
                            capture_output=True, text=True)
        self.assertEqual(cp.returncode, 0, cp.stderr)
        run = subprocess.run([str(executable)], input=data, text=True,
                             capture_output=True, timeout=20)
        if expect_error:
            self.assertNotEqual(run.returncode, 0, run.stdout)
            self.assertTrue(run.stderr)
        else:
            self.assertEqual(run.returncode, 0, run.stderr)
        return run.stdout

    def test_all_components_include_in_one_translation_unit(self):
        source = 'int main(void) { return 0; }\n'
        self.assertEqual(self.run_c(source), "")

    def test_math_bits_sieve_and_complex(self):
        code = r"""
int main(void) {
    unsigned char sieve[101];
    sieve_primes(sieve, 100);
    Complex a=complex_new(1,2), b=complex_new(3,4);
    Complex c=complex_mul(a,b);
    Complex d=complex_div(c,b);
    if (!sieve[97] || sieve[1] || !is_prime(97) || is_prime(91)) return 2;
    if (gcd(-12,18)!=6 || lcm(-12,18)!=36 || int_pow(-2,3)!=-8 ||
        factorial_basic(20)!=2432902008176640000LL || mod_pow(2,10,1000)!=24) return 3;
    if (!nearly_equal(.1+.2,.3,1e-12,1e-12)) return 4;
    if (bit_count(13)!=3 || bit_get(13,2)!=1 || bit_rotate_left(1ULL,1)!=2) return 5;
    if (!nearly_equal(d.re,1,1e-12,0) || !nearly_equal(d.im,2,1e-12,0)) return 6;
    print_int(123,"\n");
    return 0;
}"""
        self.assertEqual(self.run_c(code, selected={
            "math_integer","math_real","prime_sieve","bits","complex","print_scalar"}), "123\n")

    def test_arrays_and_strings(self):
        code = r"""
int main(void) {
    int a[8], b[8];
    if (values_ints(a,5,3,5,1)!=4) return 1;
    copy_ints(b,a,4);
    sort_ints(a,4);
    if (a[0]!=1 || a[1]!=3 || a[2]!=5 || a[3]!=5) return 2;
    if (binary_search_ints(a,4,3)!=1 || lower_bound_ints(a,4,5)!=2 ||
        upper_bound_ints(a,4,5)!=4 || find_ints(b,4,5)!=0) return 3;
    if (unique_sorted_ints(a,4)!=3) return 4;
    if (range_ints(a,1,10,2)!=5 || a[4]!=9) return 5;
    fill_ints(b,3,7);
    reverse_ints(b,3);
    if (sum_ints(b,3)!=21 || min_ints(b,3)!=7 ||
        median_ints(a,5)!=5 || max_ints(a,5)!=9) return 6;
    if (longest_plus_one_run_ints(a,5)!=1) return 7;

    char s[80]=" Hello  world ";
    str_trim(s); str_lower(s);
    if (str_count_char(s,'l')!=3 || !str_is_palindrome("abba")) return 8;
    str_insert(s,5,",");
    str_replace_all(s,"world","C99");
    if (strcmp(s,"Hello") == 0) return 9;
    str_remove_char(s,' ');
    if (strcmp(s,"hello,C99")!=0) return 10;
    str_reverse(s); str_reverse(s);
    char order[40]="banana";
    str_sort_chars(order);
    if (strcmp(order,"aaabnn")!=0) return 11;
    if (str_unique_chars(order)!=3 || strcmp(order,"abn")!=0) return 12;
    print_text(s,"\n");
    return 0;
}"""
        self.assertEqual(self.run_c(code, selected={
            "array_int_build","array_int_sort","array_int_search","array_int_stats",
            "array_int_sequences","str_basic","str_edit","str_sort","print_scalar"}),
            "hello,C99\n")

    def test_prefix_difference_enumeration_and_bases(self):
        code = r"""
int main(void) {
    int a[]={2,4,8,16};
    long long prefix[5], diff[5], values[4];
    build_prefix_ints(a,4,prefix);
    if (prefix_range_sum(prefix,1,3)!=12) return 1;
    build_diff_ints(a,4,diff);
    diff_range_add(diff,4,1,3,10);
    diff_to_lls(diff,4,values);
    if (values[0]!=2 || values[1]!=14 || values[2]!=18 || values[3]!=16) return 2;
    Combination it=combination_new(4,2);
    int combinations=0;
    while (combination_next(&it)) ++combinations;
    Permutation p=permutation_new(3,2);
    int permutations=0;
    while (permutation_next(&p)) ++permutations;
    if (combinations!=6 || permutations!=6) return 3;
    char text[80];
    int_to_base(-255,16,text);
    if (strcmp(text,"-ff")!=0 || base_to_int("-ff",16)!=-255 ||
        int_digit_sum(12345,10)!=15 || int_digit_at(12345,10,0)!=5) return 4;
    print_int(combinations+permutations,"\n");
    return 0;
}"""
        self.assertEqual(self.run_c(code, selected={
            "array_prefix","array_difference","combinations","permutations",
            "number_base","print_scalar"}), "12\n")

    def test_bigint_signed_arithmetic(self):
        code = r"""
int main(void) {
    BigInt a,b,c,q,r;
    bigint_from_str(&a,"9999999999999999999999");
    bigint_from_str(&b,"-2");
    bigint_add(&c,&a,&b);
    char out[100];
    bigint_to_str(&c,out);
    if (strcmp(out,"9999999999999999999997")!=0) return 1;
    bigint_sub(&c,&a,&b);
    bigint_to_str(&c,out);
    if (strcmp(out,"10000000000000000000001")!=0) return 2;
    bigint_mul(&c,&a,&b);
    bigint_to_str(&c,out);
    if (strcmp(out,"-19999999999999999999998")!=0) return 3;
    bigint_divmod(&q,&r,&c,&a);
    bigint_to_str(&q,out);
    if (strcmp(out,"-2")!=0 || r.sign != 0) return 4;
    bigint_from_ll(&a,-25);
    bigint_div_small(&q,(int[]){0},&a,3);
    bigint_to_str(&q,out);
    if (strcmp(out,"-8")!=0) return 5;
    bigint_mul_small(&c,&q,3);
    bigint_to_str(&c,out);
    if (strcmp(out,"-24")!=0) return 6;
    bigint_factorial(&c,10);
    bigint_to_str(&c,out);
    if (strcmp(out,"3628800")!=0) return 7;
    bigint_from_str(&b,"-3628800");
    if (bigint_compare(&b,&c)>=0) return 8;
    bigint_print(&c,"\n");
    return 0;
}"""
        self.assertEqual(self.run_c(code, selected={
            "bigint_parse","bigint_compare","bigint_add","bigint_mul",
            "bigint_div","bigint_small","bigint_factorial"}), "3628800\n")

    def test_bigint_io(self):
        source=r"""
int main(void) {
    BigInt a,b,c;
    read_bigint(&a);
    if (!try_read_bigint(&b)) return 1;
    bigint_add(&c,&a,&b);
    bigint_print(&c,"\n");
    return 0;
}"""
        self.assertEqual(self.run_c(source,"-123 456",{
            "bigint_io","bigint_add","bigint_parse"}),"333\n")

    def test_capacity_and_illegal_values(self):
        cases = [
            (r'int main(void){int a[2];fill_ints(a,3,1);}',{"array_int_build"},""),
            (r'int main(void){char s[4]="ab";str_insert(s,1,"hello");}',{"str_edit"},""),
            (r'int main(void){BigInt a;bigint_from_str(&a,"+");}',{"bigint_parse"},""),
            (r'int main(void){BigInt a,b,q,r;bigint_zero(&a);bigint_zero(&b);bigint_divmod(&q,&r,&a,&b);}',{"bigint_div"},""),
            (r'int main(void){int a[2];range_ints(a,0,100,1);}',{"array_int_build"},""),
        ]
        for code, selected, data in cases:
            with self.subTest(code=code):
                self.run_c(code, data, selected, expect_error=True)


    def test_dates_times_typed_arrays_and_digit_operations(self):
        source = r"""
int main(void) {
    Date leap=date_new(2024,2,29);
    Date next=date_add_days(leap,1);
    Date start=date_from_serial(0);
    if(!date_is_leap(2024) || date_is_leap(2100) ||
       next.month!=3 || next.day!=1 || next.year!=2024 ||
       start.year!=1 || start.month!=1 || start.day!=1 ||
       date_diff(leap,next)!=1) return 1;
    Time t=time_new(23,59,59);
    Time t2=time_next_second(t);
    if(t2.hour!=0 || t2.minute!=0 || t2.second!=0 ||
       time_diff(time_new(1,0,0),time_new(2,0,0))!=3600) return 2;
    long long large[5];
    fill_lls(large,5,-7);
    large[0]=9;
    sort_lls(large,5);
    if(large[0]!=-7 || large[4]!=9 || sum_lls(large,5)!=-19 ||
       lower_bound_lls(large,5,9)!=4) return 3;
    double doubles[5];
    fill_doubles(doubles,5,2.5);
    doubles[0]=4.0;
    sort_doubles(doubles,5);
    if(doubles[4]!=4.0 || sum_doubles(doubles,5)!=14.0 ||
       find_doubles(doubles,5,4.0)!=4) return 4;
    int digits[20];
    int len=int_to_digits(255,16,digits);
    if(len!=2 || digits[0]!=15 || digits[1]!=15 ||
       digits_to_int(digits,len,16)!=255 ||
       int_reverse_digits(-1200)!=-21) return 5;
    date_print(next," ");
    time_print(t2,"\n");
    return 0;
}"""
        self.assertEqual(self.run_c(source, selected={
            "date","time","array_ll_basic","array_double_basic",
            "number_digits_extra"}), "2024-03-01 00:00:00\n")

    def test_bigint_remainders_and_boundaries(self):
        source=r"""
int main(void) {
    BigInt a,b,c,q,r;
    bigint_from_str(&a,"-100000000000000000000000000000000");
    bigint_from_str(&b,"3");
    bigint_divmod(&q,&r,&a,&b);
    char qs[80],rs[80];
    bigint_to_str(&q,qs);
    bigint_to_str(&r,rs);
    if(strcmp(qs,"-33333333333333333333333333333333") ||
       strcmp(rs,"-1")) return 1;
    bigint_from_str(&a,"0000000000");
    if(a.sign!=0 || a.len!=1) return 2;
    bigint_from_ll(&b,LLONG_MIN);
    bigint_to_str(&b,qs);
    if(strcmp(qs,"-9223372036854775808")) return 3;
    bigint_factorial(&c,25);
    bigint_to_str(&c,qs);
    if(strcmp(qs,"15511210043330985984000000")) return 4;
    print_int(1,"\n");
    return 0;
}"""
        self.assertEqual(self.run_c(source, selected={
            "bigint_parse","bigint_div","bigint_factorial","print_scalar"
        }),"1\n")


    def test_polynomial_bisection_and_frequency(self):
        source=r"""
static double f(double x) { return x*x - 2.0; }
int main(void) {
    long long coeff[]={1,2,3};
    int values[]={0,1,1,3,3,3};
    int freq[5];
    frequency_ints(values,6,freq,4);
    if(freq[0]!=1 || freq[1]!=2 || freq[2]!=0 || freq[3]!=3) return 1;
    if(poly_eval_lls(coeff,3,2)!=17) return 2;
    double root=bisect_root(f,0,2,1e-11,200);
    if(fabs(root-sqrt(2.0))>1e-9) return 3;
    return 0;
}"""
        self.assertEqual(self.run_c(source, selected={
            "polynomial","bisect_root","array_frequency"}),"")

    def test_bigint_deterministic_randomized_arithmetic(self):
        # Compare signed arithmetic to Python's arbitrary-precision integers.
        rng=random.Random(41027)
        pairs=[]
        for _ in range(24):
            x=rng.randrange(-10**90,10**90)
            y=rng.randrange(-10**45,10**45)
            if y==0:
                y=1
            pairs.append((x,y))
        data=str(len(pairs))+"\n"+"".join(f"{x} {y}\n" for x,y in pairs)
        source=r"""
int main(void) {
    int t=read_int();
    for(int i=0;i<t;++i) {
        BigInt a,b,z,q,r;
        read_bigint(&a); read_bigint(&b);
        bigint_add(&z,&a,&b); bigint_print(&z," ");
        bigint_sub(&z,&a,&b); bigint_print(&z," ");
        bigint_mul(&z,&a,&b); bigint_print(&z," ");
        bigint_divmod(&q,&r,&a,&b);
        bigint_print(&q," "); bigint_print(&r,"\n");
    }
    return 0;
}"""
        expected=[]
        for x,y in pairs:
            q=(abs(x)//abs(y))*(1 if (x<0)==(y<0) else -1)
            r=x-q*y
            expected.append(f"{x+y} {x-y} {x*y} {q} {r}")
        self.assertEqual(self.run_c(source, data, selected={
            "io_int","bigint_io","bigint_parse","bigint_add",
            "bigint_mul","bigint_div"}), "\n".join(expected)+"\n")

    def test_bigint_factorial_large(self):
        source=r"""
int main(void) {
    BigInt value;
    bigint_factorial(&value,500);
    bigint_print(&value,"\n");
    return 0;
}"""
        self.assertEqual(self.run_c(source,selected={"bigint_factorial","bigint_parse"}),
                         str(math.factorial(500))+"\n")


if __name__ == "__main__":
    if not shutil.which(CC):
        raise SystemExit("C compiler not found: " + CC)
    unittest.main()
