//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// diff.cpp
//
// Code generation for function 'diff'
//

// Include files
#include "diff.h"

// Function Definitions
//
//
namespace coder {
void diff(const double x_data[], const int x_size[2], double y_data[],
          int y_size[2])
{
  int dimSize;
  dimSize = x_size[1];
  if (x_size[1] == 0) {
    y_size[0] = 1;
    y_size[1] = 0;
  } else {
    int u0;
    u0 = x_size[1] - 1;
    if (u0 > 1) {
      u0 = 1;
    }
    if (u0 < 1) {
      y_size[0] = 1;
      y_size[1] = 0;
    } else {
      y_size[0] = 1;
      y_size[1] = x_size[1] - 1;
      if (x_size[1] - 1 != 0) {
        double work_data;
        work_data = x_data[0];
        for (u0 = 2; u0 <= dimSize; u0++) {
          double d;
          double tmp1;
          tmp1 = x_data[u0 - 1];
          d = tmp1;
          tmp1 -= work_data;
          work_data = d;
          y_data[u0 - 2] = tmp1;
        }
      }
    }
  }
}

} // namespace coder

// End of code generation (diff.cpp)
