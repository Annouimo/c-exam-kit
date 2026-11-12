Complex complex_div(Complex a, Complex b) {
    double den = b.re*b.re + b.im*b.im;
    Complex c = {(a.re*b.re + a.im*b.im)/den,
                 (a.im*b.re - a.re*b.im)/den};
    return c;
}
