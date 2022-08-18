/* Copyright 2018-2020 The MathWorks, Inc. */


#ifndef UNPACK_CX_H
#define UNPACK_CX_H

#include "coder_cs.h"

#ifdef __cplusplus
extern "C" {
#endif

void unpackCXSparse_ci(cs_ci* in, cs_complex_t* d, int* colidx, int* rowidx);
int getNZmax_ci(cs_ci* in);
int getNNZ_ci(cs_ci* in);
int getM_ci(cs_ci* in);
int getN_ci(cs_ci* in);
cs_ci* getCholPointer_ci(cs_cin* decomp);


void unpackCXSparse_di(cs_di* in, double* d, int* colidx, int* rowidx);
int getNZmax_di(cs_di* in);
int getNNZ_di(cs_di* in);
int getM_di(cs_di* in);
int getN_di(cs_di* in);
cs_di* getCholPointer_di(cs_din* decomp);

#ifdef __cplusplus
}
#endif

#endif
