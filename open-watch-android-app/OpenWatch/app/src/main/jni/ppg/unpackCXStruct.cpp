
/* Copyright 2018 The MathWorks, Inc. */
#include "unpackCXStruct.h"
#include <string.h>

void unpackCXSparse_ci(cs_ci* in, cs_complex_t* d, int* colidx, int* rowidx) {
    int col;
    int cxIdx;
    int outIdx = 0;
    colidx[0] = 1;
    cs_complex_t ZERO = cs_createFromDouble(0);
    for (col = 1; col <= in->n; col++) {
        for (cxIdx = in->p[col - 1]; cxIdx < in->p[col]; cxIdx++) {
            if (!cs_equalcc(in->x[cxIdx], ZERO)) {
                d[outIdx] = in->x[cxIdx];
                rowidx[outIdx] = in->i[cxIdx] + 1;
                outIdx++;
            }
        }
        colidx[col] = outIdx + 1;
    }
}

int getNZmax_ci(cs_ci* in) {
    return in->nzmax;
}

int getNNZ_ci(cs_ci* in) {
    return in->p[in->n];
}

int getM_ci(cs_ci* in) {
    return in->m;
}

int getN_ci(cs_ci* in) {
    return in->n;
}

cs_ci* getCholPointer_ci(cs_cin* decomp) {
    return decomp->L;
}

void unpackCXSparse_di(cs_di* in, double* d, int* colidx, int* rowidx) {
    int col;
    int cxIdx;
    int outIdx = 0;
    colidx[0] = 1;
    for (col = 1; col <= in->n; col++) {
        for (cxIdx = in->p[col - 1]; cxIdx < in->p[col]; cxIdx++) {
            if (in->x[cxIdx] != 0.0) {
                d[outIdx] = in->x[cxIdx];
                rowidx[outIdx] = in->i[cxIdx] + 1;
                outIdx++;
            }
        }
        colidx[col] = outIdx + 1;
    }
}

int getNZmax_di(cs_di* in) {
    return in->nzmax;
}

int getNNZ_di(cs_di* in) {
    return in->p[in->n];
}

int getM_di(cs_di* in) {
    return in->m;
}

int getN_di(cs_di* in) {
    return in->n;
}

cs_di* getCholPointer_di(cs_din* decomp) {
    return decomp->L;
}