/* Copyright 2018 The MathWorks, Inc. */
#include "solve_from_qr.h"
#include <float.h>

int qr_rank_ci(cs_cin* N, double* tol) {
    int col, rowOffset, nnzThisCol, row, minSize, maxSize, rank;
    double eps, scale;
    cs_ci* mat = N->U; /* R, from QR */
    int* p = mat->p;
    if (mat->m < mat->n) {
        minSize = mat->m;
        maxSize = mat->n;
    } else {
        minSize = mat->n;
        maxSize = mat->m;
    }

    if (minSize == 0 || mat->nzmax == 0) {
        *tol = 0;
        return 0;
    }

    eps = DBL_EPSILON;
    scale = fmin(10 * maxSize * eps, sqrt(eps));
    *tol = scale * CS_ABS(mat->x[0]);

    rank = 0;
    for (col = 0; col < minSize; col++) {
        nnzThisCol = p[col + 1] - p[col];
        if (col == 0 && nnzThisCol == 0) {
            *tol = 0;
            return 0;
        }

        for (rowOffset = 0; rowOffset < nnzThisCol; rowOffset++) {
            row = mat->i[p[col] + rowOffset];
            if (row == col) {
                if (CS_ABS(mat->x[p[col] + rowOffset]) > *tol) {
                    rank = rank + 1;
                    break;
                } else {
                    break;
                }
            } else if (row > col) {
                break;
            }
        }
    }
    return rank;
}

int qr_rank_di(cs_din* N, double* tol) {
    int col, rowOffset, nnzThisCol, row, minSize, maxSize, rank;
    double eps, scale;
    cs_di* mat = N->U; /* R, from QR */
    int* p = mat->p;
    if (mat->m < mat->n) {
        minSize = mat->m;
        maxSize = mat->n;
    } else {
        minSize = mat->n;
        maxSize = mat->m;
    }

    if (minSize == 0 || mat->nzmax == 0) {
        *tol = 0;
        return 0;
    }

    eps = DBL_EPSILON;
    scale = fmin(10 * maxSize * eps, sqrt(eps));
    *tol = scale * fabs(mat->x[0]);

    rank = 0;
    for (col = 0; col < minSize; col++) {
        nnzThisCol = p[col + 1] - p[col];
        if (col == 0 && nnzThisCol == 0) {
            *tol = 0;
            return 0;
        }

        for (rowOffset = 0; rowOffset < nnzThisCol; rowOffset++) {
            row = mat->i[p[col] + rowOffset];
            if (row == col) {
                if (fabs(mat->x[p[col] + rowOffset]) > *tol) {
                    rank = rank + 1;
                    break;
                } else {
                    return rank;
                }
            } else if (row > col) {
                return rank;
            }
        }
    }
    return rank;
}

void solve_from_qr_di(cs_din* N, cs_dis* S, double* b, int m, int n) {
    double* x = (double*)cs_di_calloc(S ? S->m2 : 1, sizeof(double)); /* get workspace */
    if (m >= n) {
        cs_di_ipvec(S->pinv, b, x, m); /* x(0:m-1) = b(p(0:m-1) */
        CS_INT k;
        for (k = 0; k < n; k++) /* apply Householder refl. to x */
        {
            cs_di_happly(N->L, k, N->B[k], x);
        }
        cs_di_usolve(N->U, x);      /* x = R\x */
        cs_di_ipvec(S->q, x, b, n); /* b(q(0:n-1)) = x(0:n-1) */

    } else {
        cs_di_pvec(S->q, b, x, m); /* x(q(0:m-1)) = b(0:m-1) */
        cs_di_utsolve(N->U, x);    /* x = R'\x */
        CS_INT k;
        for (k = m - 1; k >= 0; k--) /* apply Householder refl. to x */
        {
            cs_di_happly(N->L, k, N->B[k], x);
        }
        cs_di_pvec(S->pinv, x, b, n); /* b(0:n-1) = x(p(0:n-1)) */
    }
    cs_di_free(x);
}

void solve_from_qr_ci(cs_cin* N, cs_cis* S, cs_complex_t* b, int m, int n) {
    cs_complex_t* x =
        (cs_complex_t*)cs_ci_calloc(S ? S->m2 : 1, sizeof(cs_complex_t)); /* get workspace */
    if (m >= n) {
        cs_ci_ipvec(S->pinv, b, x, m); /* x(0:m-1) = b(p(0:m-1) */
        CS_INT k;
        for (k = 0; k < n; k++) /* apply Householder refl. to x */
        {
            cs_ci_happly(N->L, k, N->B[k], x);
        }
        cs_ci_usolve(N->U, x);      /* x = R\x */
        cs_ci_ipvec(S->q, x, b, n); /* b(q(0:n-1)) = x(0:n-1) */

    } else {
        cs_ci_pvec(S->q, b, x, m); /* x(q(0:m-1)) = b(0:m-1) */
        cs_ci_utsolve(N->U, x);    /* x = R'\x */
        CS_INT k;
        for (k = m - 1; k >= 0; k--) /* apply Householder refl. to x */
        {
            cs_ci_happly(N->L, k, N->B[k], x);
        }
        cs_ci_pvec(S->pinv, x, b, n); /* b(0:n-1) = x(p(0:n-1)) */
    }
    cs_ci_free(x);
}
