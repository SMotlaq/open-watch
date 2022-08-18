/* Copyright 2018 The MathWorks, Inc. */
#include "makeCXSparseMatrix.h"
#include <string.h>

cs_di* makeCXSparseMatrix(int nnz,
                       int n,
                       int m,
                       const int* colidx,
                       const int* rowidx,
                       const double* x) {
    size_t idx;
    cs_di* T;

    T = cs_di_spalloc((CS_INT)m, (CS_INT)n, (CS_INT)nnz, 1, 0);
    memcpy(T->x, &x[0], nnz * sizeof(double));

    for (idx = 0; idx < (size_t)nnz; idx++) {
        T->i[idx] = (CS_INT)(rowidx[idx] - 1);
    }

    for (idx = 0; idx <= (size_t)n; idx++) {
        T->p[idx] = (CS_INT)(colidx[idx] - 1);
    }

    return T;
}

cs_ci* makeComplexCXSparseMatrix(int nnz,
                              int n,
                              int m,
                              const int* colidx,
                              const int* rowidx,
                              const creal_T* xi) {
    size_t idx;
    cs_ci* T;

    T = cs_ci_spalloc((CS_INT)m, (CS_INT)n, (CS_INT)nnz, 1, 0);
    memcpy(T->x, &xi[0], nnz * sizeof(creal_T));

    for (idx = 0; idx < (size_t)nnz; ++idx) {
        T->i[idx] = (CS_INT)(rowidx[idx] - 1);
    }

    for (idx = 0; idx <= (size_t)n; idx++) {
        T->p[idx] = (CS_INT)(colidx[idx] - 1);
    }

    return T;
}
