/* Copyright 2018-2020 The MathWorks, Inc. */
#ifndef _MAKECXSPARSEMATRIX_H
#define _MAKECXSPARSEMATRIX_H
#include "coder_cs.h"
#include "rtwtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

cs_di* makeCXSparseMatrix(int nnz,
                       int n,
                       int m,
                       const int* colidx,
                       const int* rowidx,
                       const double* x);

cs_ci* makeComplexCXSparseMatrix(int nnz,
                              int n,
                              int m,
                              const int* colidx,
                              const int* rowidx,
                              const creal_T* xi);


#ifdef __cplusplus
}
#endif
#endif /*_MAKECXSPARSEMATRIX_H*/
