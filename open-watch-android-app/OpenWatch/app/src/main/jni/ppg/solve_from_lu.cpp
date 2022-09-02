/* Copyright 2018 The MathWorks, Inc. */
#include "solve_from_lu.h"

int solve_from_lu_ci(cs_cin* N, cs_cis* S, cs_complex_t* b, int n) {
    cs_complex_t* x = (cs_complex_t*)cs_ci_malloc(n, sizeof(cs_complex_t));

    cs_ci_ipvec(N->pinv, b, x, n); /* x = b(p) */
    cs_ci_lsolve(N->L, x);         /* x = L\x */
    cs_ci_usolve(N->U, x);         /* x = U\x */
    cs_ci_ipvec(S->q, x, b, n);    /* b(q) = x */
    cs_ci_free(x);

    return 0;
}

int solve_from_lu_di(cs_din* N, cs_dis* S, double* b, int n) {
    double* x = (double*)cs_di_malloc(n, sizeof(double));

    cs_di_ipvec(N->pinv, b, x, n); /* x = b(p) */
    cs_di_lsolve(N->L, x);         /* x = L\x */
    cs_di_usolve(N->U, x);         /* x = U\x */
    cs_di_ipvec(S->q, x, b, n);    /* b(q) = x */
    cs_di_free(x);

    return 0;
}
