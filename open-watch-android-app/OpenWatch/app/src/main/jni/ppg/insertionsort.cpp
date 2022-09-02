//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// insertionsort.cpp
//
// Code generation for function 'insertionsort'
//

// Include files
#include "insertionsort.h"
#include "PPG_Algorithm_data.h"

// Function Definitions
//
//
namespace coder {
namespace internal {
void insertionsort(int x[301], int xstart, int xend)
{
  int i;
  i = xstart + 1;
  for (int k{i}; k <= xend; k++) {
    int idx;
    int xc;
    boolean_T exitg1;
    xc = x[k - 1];
    idx = k - 1;
    exitg1 = false;
    while ((!exitg1) && (idx >= xstart)) {
      int aj;
      int i1;
      boolean_T varargout_1;
      aj = iv[x[idx - 1] - 1];
      i1 = iv[xc - 1];
      if (i1 < aj) {
        varargout_1 = true;
      } else if (i1 == aj) {
        varargout_1 = (iv1[xc - 1] < iv1[x[idx - 1] - 1]);
      } else {
        varargout_1 = false;
      }
      if (varargout_1) {
        x[idx] = x[idx - 1];
        idx--;
      } else {
        exitg1 = true;
      }
    }
    x[idx] = xc;
  }
}

} // namespace internal
} // namespace coder

// End of code generation (insertionsort.cpp)
