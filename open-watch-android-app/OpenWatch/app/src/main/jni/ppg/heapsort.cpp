//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// heapsort.cpp
//
// Code generation for function 'heapsort'
//

// Include files
#include "heapsort.h"
#include "PPG_Algorithm_data.h"

// Function Declarations
namespace coder {
namespace internal {
static void heapify(int x[301], int idx, int xstart, int xend);

}
} // namespace coder

// Function Definitions
//
//
namespace coder {
namespace internal {
static void heapify(int x[301], int idx, int xstart, int xend)
{
  int ai;
  int aj;
  int extremum;
  int extremumIdx;
  int leftIdx;
  boolean_T changed;
  boolean_T exitg1;
  boolean_T varargout_1;
  changed = true;
  extremumIdx = (idx + xstart) - 2;
  leftIdx = ((idx << 1) + xstart) - 2;
  exitg1 = false;
  while ((!exitg1) && (leftIdx + 1 < xend)) {
    int aj_tmp;
    int cmpIdx;
    int xcmp;
    changed = false;
    extremum = x[extremumIdx];
    cmpIdx = leftIdx;
    xcmp = x[leftIdx];
    ai = iv[x[leftIdx] - 1];
    aj_tmp = x[leftIdx + 1] - 1;
    aj = iv[aj_tmp];
    if (ai < aj) {
      varargout_1 = true;
    } else if (ai == aj) {
      varargout_1 = (iv1[x[leftIdx] - 1] < iv1[aj_tmp]);
    } else {
      varargout_1 = false;
    }
    if (varargout_1) {
      cmpIdx = leftIdx + 1;
      xcmp = x[leftIdx + 1];
    }
    ai = iv[x[extremumIdx] - 1];
    aj_tmp = iv[xcmp - 1];
    if (ai < aj_tmp) {
      varargout_1 = true;
    } else if (ai == aj_tmp) {
      varargout_1 = (iv1[x[extremumIdx] - 1] < iv1[xcmp - 1]);
    } else {
      varargout_1 = false;
    }
    if (varargout_1) {
      x[extremumIdx] = xcmp;
      x[cmpIdx] = extremum;
      extremumIdx = cmpIdx;
      leftIdx = ((((cmpIdx - xstart) + 2) << 1) + xstart) - 2;
      changed = true;
    } else {
      exitg1 = true;
    }
  }
  if (changed && (leftIdx + 1 <= xend)) {
    extremum = x[extremumIdx];
    ai = iv[x[extremumIdx] - 1];
    aj = iv[x[leftIdx] - 1];
    if (ai < aj) {
      varargout_1 = true;
    } else if (ai == aj) {
      varargout_1 = (iv1[x[extremumIdx] - 1] < iv1[x[leftIdx] - 1]);
    } else {
      varargout_1 = false;
    }
    if (varargout_1) {
      x[extremumIdx] = x[leftIdx];
      x[leftIdx] = extremum;
    }
  }
}

//
//
void b_heapsort(int x[301], int xstart, int xend)
{
  int idx;
  int n;
  n = (xend - xstart) - 1;
  for (idx = n + 2; idx >= 1; idx--) {
    heapify(x, idx, xstart, xend);
  }
  for (int k{0}; k <= n; k++) {
    int t;
    idx = (xend - k) - 1;
    t = x[idx];
    x[idx] = x[xstart - 1];
    x[xstart - 1] = t;
    heapify(x, 1, xstart, (xend - k) - 1);
  }
}

} // namespace internal
} // namespace coder

// End of code generation (heapsort.cpp)
