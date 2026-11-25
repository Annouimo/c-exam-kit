"""Cross-platform compile/run tests for standalone C99 algorithm preset."""
from pathlib import Path
import json
import random
import re
import math
import os
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
PRESET = ROOT / "npm" / "accoding-preset" / "preset"
subprocess.run(["node", str(PRESET.parent / "scripts" / "build-preset.mjs")], check=True)
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
    lines.extend(c["code"]
                 for c in MANIFEST["components"] if c["id"] in selected)
    lines.append("#endif")
    return "\n".join(lines) + "\n"


class Algorithms(unittest.TestCase):
    def test_record_search_manual_example(self):
        reference = json.loads((PRESET / "reference.json").read_text(encoding="utf-8"))
        example = next(entry["example"] for category in reference["categories"]
                       for entry in category["entries"] if entry["id"] == "find_records_by")
        self.assertEqual(self.run_c(example), "0\n1\n")

    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="algorithms-c99-")
        self.addCleanup(self.temp.cleanup)
        self.folder = Path(self.temp.name)

    def run_c(self, source, data="", selected=None, expect_error=False, diagnostic=()):
        (self.folder/"exam.h").write_text(header(set(re.findall(r"[A-Za-z_]\w*", source)) & COMPONENTS.keys() if selected is not None else None), encoding="utf-8")
        (self.folder/"main.c").write_text('#include "exam.h"\n'+source,
                                          encoding="utf-8")
        executable = self.folder/("test.exe" if os.name=="nt" else "test")
        cp = subprocess.run([CC, "-std=c99", "-O2", "-Wall", "-Wextra",
                             "-Werror", str(self.folder/"main.c"),
                             "-o", str(executable), "-lm"],
                            capture_output=True, text=True)
        self.assertEqual(cp.returncode, 0, cp.stderr)
        run = subprocess.run([str(executable)], input=data, text=True,
                             capture_output=True, encoding="utf-8", timeout=20)
        if expect_error:
            self.assertNotEqual(run.returncode, 0, run.stdout)
            self.assertTrue(run.stderr)
            for part in diagnostic:
                self.assertIn(part, run.stderr)
            if diagnostic:
                self.assertEqual(len(run.stderr.splitlines()), 1)
                self.assertEqual(run.stdout, "")
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


    def test_calendar_format_and_read(self):
        source = r"""
int main(void) {
    Date d = date_read();
    Time t = time_read();
    date_print(d, "%d/%m/%Y", "\n");
    date_print(d, "%Y年%-m月%-d日 %m%m %%", "\n");
    time_print(t, "%-H:%M:%S", "\n");
    date_print(date_new(1, 2, 3), "%Y/%-Y/%m/%d", "\n");
    return 0;
}"""
        self.assertEqual(self.run_c(source, "2024 02 09 8 5 3\n", True),
                         "09/02/2024\n2024年2月9日 0202 %\n8:05:03\n0001/1/02/03\n")

    def test_calendar_and_common_component_diagnostics(self):
        cases = [
            ('date_add_days((Date){0, 0, 0}, 2);', '',
             ('date_add_days', 'year=0, month=0, day=0', 'delta=2')),
            ('date_add_days(date_new(9999, 12, 31), 1);', '',
             ('date_add_days', 'outside', '9999-12-31', 'delta=1')),
            ('date_new(1900, 2, 29);', '', ('date_new', 'day=29', '1900-02', '1..28')),
            ('date_to_serial((Date){INT_MIN, 1, 1});', '', ('date_to_serial', 'year=', '1..9999')),
            ('date_diff(date_new(2000, 1, 1), (Date){2000, 13, 1});', '',
             ('date_diff', 'b.month=13', '1..12')),
            ('date_print(date_new(2000, 1, 1), "%", "\\n");', '',
             ('date_print', 'incomplete', 'byte 0')),
            ('time_print(time_new(1, 2, 3), "%m", "\\n");', '',
             ('time_print', 'unsupported', 'byte 0', '0x6D')),
            ('time_next_minute((Time){24, 0, 0});', '', ('time_next_minute', 'hour=24')),
            ('date_read();', '2000 2', ('date_read', 'EOF', 'day')),
            ('time_read();', '8 0 x', ('time_read', 'second', 'token="x"')),
            ('char s[4] = "ab"; str_insert(s, 3, "cd");', '',
             ('str_insert', 'pos=3', '0..2')),
            ('char s[4] = "ab"; str_insert(s, 1, "cd");', '',
             ('str_insert', 'capacity=4', 'inserted=2')),
            ('int a[2] = {0}; long long prefix[2]; build_prefix_ints(a, 2, prefix);', '',
             ('build_prefix_ints', 'capacity=2', 'need 3 elements')),
        ]
        for statement, data, diagnostic in cases:
            with self.subTest(statement=statement):
                self.run_c("int main(void) { " + statement + " return 0; }",
                           data, True, expect_error=True, diagnostic=diagnostic)

    def test_record_search_null_comparator(self):
        for operation in ("find_records_by", "binary_search_records_by"):
            with self.subTest(operation=operation):
                source = ("struct Record { int id; }; int main(void) { "
                          f"Record key = {{1}}; return {operation}(&key, 1, &key, NULL); }}")
                self.run_c(source, selected=True, expect_error=True,
                           diagnostic=(operation, "compare must not be NULL", "n=1"))

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
    date_print(next,"%Y-%m-%d"," ");
    time_print(t2,"%H:%M:%S","\n");
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


    def test_record_sort_and_stable_argsort(self):
        source = r"""
struct Record {
    char name[20];
    int score;
    int penalty;
};
static int cmp_rank(const Record *a, const Record *b) {
    if (a->score != b->score) return (a->score < b->score) - (a->score > b->score);
    if (a->penalty != b->penalty) return (a->penalty > b->penalty) - (a->penalty < b->penalty);
    return strcmp(a->name, b->name);
}
static int cmp_name(const Record *a, const Record *b) { return strcmp(a->name,b->name); }
int main(void) {
    Record records[]={{"J",90,5},{"A",100,20},{"C",100,10},{"B",100,10}};
    sort_records(records,4,cmp_rank);
    if(strcmp(records[0].name,"B") || strcmp(records[1].name,"C") ||
       strcmp(records[2].name,"A") || strcmp(records[3].name,"J")) return 1;
    Record key = {"C",100,10};
    if (binary_search_records_by(records,4,&key,cmp_rank)!=1) return 4;
    sort_records(records,4,cmp_name);
    if(strcmp(records[0].name,"A") || strcmp(records[3].name,"J")) return 5;
    int values[]={8,3,8,2}, order[4];
    argsort_ints(values,4,order);
    if(order[0]!=3 || order[1]!=1 || order[2]!=0 || order[3]!=2) return 2;
    if(values[0]!=8 || values[1]!=3 || values[2]!=8 || values[3]!=2) return 3;
    return 0;
}"""
        self.assertEqual(self.run_c(source, selected={"sort_records", "argsort_ints",
                                                     "search_records"}), "")

    def test_record_search_without_sorting_callback(self):
        source = r"""
struct Record {
    char name[20];
    int id;
};

static int by_id_desc(const Record *a, const Record *b) {
    return (a->id < b->id) - (a->id > b->id);
}

int main(void) {
    const Record unsorted[] = {{"A",2}, {"B",5}, {"C",2}, {"D",9}};
    const Record sorted[] = {{"D",9}, {"B",5}, {"A",2}, {"C",2}};
    const Record key = {.id = 2};
    if (find_records_by(unsorted, 4, &key, by_id_desc) != 0) return 1;
    if (binary_search_records_by(sorted, 4, &key, by_id_desc) != 2) return 2;
    const Record missing = {.id = 6};
    if (find_records_by(unsorted, 4, &missing, by_id_desc) != -1) return 3;
    if (binary_search_records_by(sorted, 4, &missing, by_id_desc) != -1) return 4;
    if (binary_search_records_by(sorted, 4, &(Record){.id = 1}, by_id_desc) != -1) return 5;
    if (find_records_by(unsorted, 0, &key, by_id_desc) != -1) return 6;
    if (binary_search_records_by(sorted, 0, &key, by_id_desc) != -1) return 7;
    return 0;
}"""
        self.assertEqual(self.run_c(source, selected={"search_records"}), "")

    def test_split_and_join_strict_rectangular_arrays(self):
        source = r"""
int main(void) {
    char words[8][20], out[128];
    int n=str_split("a,,b,",",",words);
    if(n!=4 || strcmp(words[0],"a") || strcmp(words[1],"") ||
       strcmp(words[2],"b") || strcmp(words[3],"")) return 1;
    str_join(words,n,",",out);
    if(strcmp(out,"a,,b,")) return 2;
    n=str_split("one::two::::","::",words);
    if(n!=4 || strcmp(words[0],"one") || strcmp(words[1],"two") ||
       strcmp(words[2],"") || strcmp(words[3],"")) return 3;
    str_join(words,n,"::",out);
    if(strcmp(out,"one::two::::")) return 4;
    n=str_split_whitespace("  red\tblue\n green  ",words);
    if(n!=3 || strcmp(words[0],"red") || strcmp(words[1],"blue") ||
       strcmp(words[2],"green")) return 5;
    str_join(words,n,"/",out);
    if(strcmp(out,"red/blue/green")) return 6;
    n=str_split("",",",words);
    if(n!=1 || strcmp(words[0],"")) return 7;
    n=str_split_whitespace(" \n\t  ",words);
    if(n!=0) return 8;
    str_join(words,0,",",out);
    if(strcmp(out,"")) return 9;
    print_text("ok","\n");
    return 0;
}"""
        self.assertEqual(self.run_c(source, selected={
            "str_split_join", "print_scalar"}), "ok\n")

    def test_first_true_range_and_extremes(self):
        source = r"""
static bool predicate(int x) { return x >= 7; }
static bool never(int x) { (void)x; return false; }
static bool always(int x) { (void)x; return true; }
int main(void) {
    if(first_true_int(0,10,predicate)!=7) return 1;
    if(first_true_int(-10,10,predicate)!=7) return 2;
    if(first_true_int(-10,10,never)!=10) return 3;
    if(first_true_int(-10,10,always)!=-10) return 4;
    if(first_true_int(INT_MIN,INT_MAX,predicate)!=7) return 5;
    if(first_true_int(2,2,predicate)!=2) return 6;
    if(first_true_int(INT_MIN,INT_MIN,predicate)!=INT_MIN) return 7;
    return 0;
}"""
        self.assertEqual(self.run_c(source, selected={"first_true"}), "")

    def test_long_long_and_double_complete_core(self):
        source = r"""
int main(void) {
    long long a[12], original[12];
    int n=values_lls(a,7LL,5LL,5LL,3LL);
    if(n!=4 || mean_lls(a,n)!=5 || variance_lls(a,n)!=2 ||
       fabs(stddev_lls(a,n)-sqrt(2.0))>1e-12) return 1;
    copy_lls(original,a,n);
    if(argmin_lls(a,n)!=3 || argmax_lls(a,n)!=0 ||
       count_lls(a,n,5LL)!=2 || median_lls(a,n)!=5) return 2;
    sort_lls(a,n);
    if(binary_search_lls(a,n,5)!=1 || lower_bound_lls(a,n,5)!=1 ||
       upper_bound_lls(a,n,5)!=3) return 3;
    n=unique_sorted_lls(a,n);
    if(n!=3 || a[0]!=3 || a[1]!=5 || a[2]!=7) return 4;
    sort_lls_desc(a,n);
    if(a[0]!=7 || a[2]!=3 || unique_lls(a,n)!=3) return 5;
    n=range_lls(a,-5,6,3);
    if(n!=4 || a[0]!=-5 || a[1]!=-2 || a[2]!=1 || a[3]!=4) return 6;
    n=range_lls(a,LLONG_MAX-1,LLONG_MAX,LLONG_MAX);
    if(n!=1 || a[0]!=LLONG_MAX-1) return 7;
    n=range_lls(a,LLONG_MIN+1,LLONG_MIN,LLONG_MIN);
    if(n!=1 || a[0]!=LLONG_MIN+1) return 8;
    double d[12];
    n=values_doubles(d,2.0,1.0,2.0,3.0);
    if(n!=4 || count_doubles(d,n,2.0)!=2 ||
       argmin_doubles(d,n)!=1 || argmax_doubles(d,n)!=3 ||
       median_doubles(d,n)!=2.0 || variance_doubles(d,n)!=.5 ||
       fabs(stddev_doubles(d,n)-sqrt(.5))>1e-12) return 9;
    sort_doubles(d,n);
    if(binary_search_doubles(d,n,2.0)!=1 ||
       lower_bound_doubles(d,n,2)!=1 || upper_bound_doubles(d,n,2)!=3) return 10;
    n=unique_sorted_doubles(d,n);
    if(n!=3 || d[0]!=1 || d[1]!=2 || d[2]!=3) return 11;
    sort_doubles_desc(d,n);
    if(d[0]!=3 || d[2]!=1 || unique_doubles(d,n)!=3) return 12;
    if(values_doubles(d,1.5,2.5)!=2 || d[0]!=1.5 || d[1]!=2.5) return 13;
    return 0;
}"""
        self.assertEqual(self.run_c(source, selected={
            "array_ll_extra", "array_double_extra"}), "")

    def test_new_capacity_checks_and_input_validation(self):
        cases = [
            (r'int main(void){int a[2]={4,3};int order[1];argsort_ints(a,2,order);}',
             {"argsort_ints"}),
            (r'int main(void){char rows[2][3];str_split("a,b,c",",",rows);}',
             {"str_split_join"}),
            (r'int main(void){char rows[2][3];str_split("abcdef",",",rows);}',
             {"str_split_join"}),
            (r'int main(void){char rows[1][4]={{"abc"}};char result[3];str_join(rows,1,":",result);}',
             {"str_split_join"}),
            (r'int main(void){char rows[1][4]={{"a"}};char result[8];str_join(rows,2,":",result);}',
             {"str_split_join"}),
            (r'int main(void){char rows[3][4];str_split("hi","",rows);}',
             {"str_split_join"}),
            (r'int main(void){first_true_int(4,3,NULL);}',
             {"first_true"}),
            (r'int main(void){long long a[2];values_lls(a,1LL,2LL,3LL);}',
             {"array_ll_extra"}),
            (r'int main(void){double a[1];values_doubles(a,1.0,2.0);}',
             {"array_double_extra"}),
            (r'int main(void){double a[2]={1.0,NAN};sort_doubles_desc(a,2);}',
             {"array_double_extra"}),
        ]
        for source, selected in cases:
            with self.subTest(source=source):
                self.run_c(source, selected=selected, expect_error=True)


if __name__ == "__main__":
    if not shutil.which(CC):
        raise SystemExit("C compiler not found: " + CC)
    unittest.main()
