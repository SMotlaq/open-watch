//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// filter.h
//
// Code generation for function 'filter'
//

#ifndef FILTER_H
#define FILTER_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace coder {
void filter(const double b_data[], const double a_data[], const double x_data[],
            const double zi_data[], double y_data[], int *y_size);

void filter(double b_data[], double a_data[],
            const ::coder::array<double, 1U> &x, const double zi[2],
            ::coder::array<double, 1U> &y);

} // namespace coder

#endif
// End of code generation (filter.h)
