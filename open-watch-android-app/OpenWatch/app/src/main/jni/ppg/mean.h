//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// mean.h
//
// Code generation for function 'mean'
//

#ifndef MEAN_H
#define MEAN_H

// Include files
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace coder {
double b_mean(const double x[4]);

double mean(const double x_data[], const int x_size[2]);

double mean(const double x[1000]);

double mean(const double x_data[], int x_size);

} // namespace coder

#endif
// End of code generation (mean.h)
