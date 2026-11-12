Complex complex_mul(Complex a, Complex b) {
    Complex c = {a.re*b.re - a.im*b.im, a.re*b.im + a.im*b.re};
    return c;
}
