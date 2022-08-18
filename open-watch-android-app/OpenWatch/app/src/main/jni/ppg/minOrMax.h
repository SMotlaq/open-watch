//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// minOrMax.h
//
// Code generation for function 'minOrMax'
//

#ifndef MINORMAX_H
#define MINORMAX_H

// Include files
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace coder {
namespace internal {
double maximum(const double x_data[], const int x_size[2]);

double maximum(const double x[1000]);

void maximum(const double x_data[], const int x_size[2], double *ex, int *idx);

void minimum(const double x_data[], const int x_size[2], double *ex, int *idx);

} // namespace internal
} // namespace coder

#endif
// End of code generation (minOrMax.h)
