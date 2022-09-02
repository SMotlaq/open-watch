//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// mean.cpp
//
// Code generation for function 'mean'
//

// Include files
#include "mean.h"

// Function Definitions
//
//
namespace coder {
double b_mean(const double x[4])
{
  return (((x[0] + x[1]) + x[2]) + x[3]) / 4.0;
}

//
//
double mean(const double x_data[], const int x_size[2])
{
  double y;
  int vlen;
  vlen = x_size[1];
  if (x_size[1] == 0) {
    y = 0.0;
  } else {
    y = x_data[0];
    for (int k{2}; k <= vlen; k++) {
      y += x_data[k - 1];
    }
  }
  y /= static_cast<double>(x_size[1]);
  return y;
}

//
//
double mean(const double x[1000])
{
  double y;
  y = x[0];
  for (int k{0}; k < 999; k++) {
    y += x[k + 1];
  }
  y /= 1000.0;
  return y;
}

//
//
double mean(const double x_data[], int x_size)
{
  double y;
  if (x_size == 0) {
    y = 0.0;
  } else {
    y = x_data[0];
    for (int k{2}; k <= x_size; k++) {
      y += x_data[k - 1];
    }
  }
  y /= static_cast<double>(x_size);
  return y;
}

} // namespace coder

// End of code generation (mean.cpp)
