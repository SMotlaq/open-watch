//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// nonzeros.cpp
//
// Code generation for function 'nonzeros'
//

// Include files
#include "nonzeros.h"

// Function Definitions
//
//
namespace coder {
void nonzeros(const double s_data[], const int s_size[2], double v_data[],
              int *v_size)
{
  int i;
  int n;
  n = s_size[1];
  *v_size = 0;
  i = s_size[1];
  for (int k{0}; k < i; k++) {
    if (s_data[k] != 0.0) {
      (*v_size)++;
    }
  }
  i = -1;
  for (int k{0}; k < n; k++) {
    double d;
    d = s_data[k];
    if (d != 0.0) {
      i++;
      v_data[i] = d;
    }
  }
}

} // namespace coder

// End of code generation (nonzeros.cpp)
