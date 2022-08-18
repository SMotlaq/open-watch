//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// filter.cpp
//
// Code generation for function 'filter'
//

// Include files
#include "filter.h"
#include "coder_array.h"
#include <algorithm>
#include <cmath>
#include <cstring>

// Function Definitions
//
//
namespace coder {
void filter(const double b_data[], const double a_data[], const double x_data[],
            const double zi_data[], double y_data[], int *y_size)
{
  *y_size = 1606;
  std::copy(&zi_data[0], &zi_data[101], &y_data[0]);
  std::memset(&y_data[101], 0, 1505U * sizeof(double));
  for (int k{0}; k < 1606; k++) {
    double as;
    int naxpy;
    int y_tmp;
    if (1606 - k < 102) {
      naxpy = 1605 - k;
    } else {
      naxpy = 101;
    }
    for (int j{0}; j <= naxpy; j++) {
      y_tmp = k + j;
      y_data[y_tmp] += x_data[k] * b_data[j];
    }
    if (1605 - k < 101) {
      naxpy = 1604 - k;
    } else {
      naxpy = 100;
    }
    as = -y_data[k];
    for (int j{0}; j <= naxpy; j++) {
      y_tmp = (k + j) + 1;
      y_data[y_tmp] += as * a_data[j + 1];
    }
  }
}

//
//
void filter(double b_data[], double a_data[],
            const ::coder::array<double, 1U> &x, const double zi[2],
            ::coder::array<double, 1U> &y)
{
  int niccp;
  int nx;
  if ((!std::isinf(a_data[0])) && (!std::isnan(a_data[0])) &&
      (!(a_data[0] == 0.0)) && (a_data[0] != 1.0)) {
    b_data[0] /= a_data[0];
    b_data[1] /= a_data[0];
    b_data[2] /= a_data[0];
    a_data[1] /= a_data[0];
    a_data[2] /= a_data[0];
    a_data[0] = 1.0;
  }
  y.set_size(x.size(0));
  nx = x.size(0);
  if (x.size(0) < 2) {
    niccp = x.size(0) - 1;
  } else {
    niccp = 1;
  }
  for (int k{0}; k <= niccp; k++) {
    y[k] = zi[k];
  }
  niccp += 2;
  for (int k{niccp}; k <= nx; k++) {
    y[k - 1] = 0.0;
  }
  for (int k{0}; k < nx; k++) {
    double as;
    int naxpy;
    int y_tmp;
    niccp = nx - k;
    if (niccp <= 3) {
      naxpy = niccp;
    } else {
      naxpy = 3;
    }
    for (int j{0}; j < naxpy; j++) {
      y_tmp = k + j;
      y[y_tmp] = y[y_tmp] + x[k] * b_data[j];
    }
    if (niccp - 1 < 2) {
      naxpy = niccp - 2;
    } else {
      naxpy = 1;
    }
    as = -y[k];
    for (int j{0}; j <= naxpy; j++) {
      y_tmp = (k + j) + 1;
      y[y_tmp] = y[y_tmp] + as * a_data[j + 1];
    }
  }
}

} // namespace coder

// End of code generation (filter.cpp)
