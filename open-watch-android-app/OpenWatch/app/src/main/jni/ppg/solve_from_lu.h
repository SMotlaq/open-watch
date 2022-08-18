/* Copyright 2018-2020 The MathWorks, Inc. */
#ifndef _SOLVE_FROM_LU_H
#define _SOLVE_FROM_LU_H
#include "coder_cs.h"
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/* double routines */
int solve_from_lu_di(cs_din* N, cs_dis* S, double* b, int n);

/* complex routines */
int solve_from_lu_ci(cs_cin* N, cs_cis* S, cs_complex_t* b, int n);

#ifdef __cplusplus
}
#endif
#endif /*_SOLVE_FROM_LU_H*/
