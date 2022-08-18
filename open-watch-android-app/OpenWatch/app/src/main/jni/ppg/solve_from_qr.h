/* Copyright 2018-2020 The MathWorks, Inc. */
#ifndef _SOLVE_FROM_QR_H
#define _SOLVE_FROM_QR_H
#include "coder_cs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* double routines */
void solve_from_qr_di(cs_din* N, cs_dis* S, double* b, int m, int n);
int qr_rank_di(cs_din* N, double* tol);

/* complex routines */
double complexAbs(CS_ENTRY in);
void solve_from_qr_ci(cs_cin* N, cs_cis* S, cs_complex_t* b, int m, int n);
int qr_rank_ci(cs_cin* N, double* tol);

#ifdef __cplusplus
}
#endif
#endif /*_SOLVE_FROM_QR_H*/
