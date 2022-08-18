//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// PPG_Algorithm.h
//
// Code generation for function 'PPG_Algorithm'
//

#ifndef PPG_ALGORITHM_H
#define PPG_ALGORITHM_H

// Include files
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
extern void PPG_Algorithm(const double rawsig_IR[1000],
                          const double rawsig_RED[1000], boolean_T gender,
                          int age, double SpO2_prev, double *HR,
                          double *SpO2, double PPG_IR[1000],
                          double PPG_RED[1000], int *b_index);

#endif
// End of code generation (PPG_Algorithm.h)
