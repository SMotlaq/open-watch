//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// minOrMax.cpp
//
// Code generation for function 'minOrMax'
//

// Include files
#include "minOrMax.h"
#include <cmath>

// Function Definitions
//
//
namespace coder {
namespace internal {
double maximum(const double x_data[], const int x_size[2])
{
  double ex;
  int idx;
  int k;
  int last;
  last = x_size[1];
  if (!std::isnan(x_data[0])) {
    idx = 1;
  } else {
    boolean_T exitg1;
    idx = 0;
    k = 2;
    exitg1 = false;
    while ((!exitg1) && (k <= last)) {
      if (!std::isnan(x_data[k - 1])) {
        idx = k;
        exitg1 = true;
      } else {
        k++;
      }
    }
  }
  if (idx == 0) {
    ex = x_data[0];
  } else {
    ex = x_data[idx - 1];
    idx++;
    for (k = idx; k <= last; k++) {
      double d;
      d = x_data[k - 1];
      if (ex < d) {
        ex = d;
      }
    }
  }
  return ex;
}

//
//
double maximum(const double x[1000])
{
  double ex;
  int idx;
  int k;
  if (!std::isnan(x[0])) {
    idx = 1;
  } else {
    boolean_T exitg1;
    idx = 0;
    k = 2;
    exitg1 = false;
    while ((!exitg1) && (k <= 1000)) {
      if (!std::isnan(x[k - 1])) {
        idx = k;
        exitg1 = true;
      } else {
        k++;
      }
    }
  }
  if (idx == 0) {
    ex = x[0];
  } else {
    ex = x[idx - 1];
    idx++;
    for (k = idx; k < 1001; k++) {
      double d;
      d = x[k - 1];
      if (ex < d) {
        ex = d;
      }
    }
  }
  return ex;
}

//
//
void maximum(const double x_data[], const int x_size[2], double *ex, int *idx)
{
  int last;
  last = x_size[1];
  if (x_size[1] <= 2) {
    if (x_size[1] == 1) {
      *ex = x_data[0];
      *idx = 1;
    } else {
      *ex = x_data[x_size[1] - 1];
      if ((x_data[0] < *ex) || (std::isnan(x_data[0]) && (!std::isnan(*ex)))) {
        *idx = x_size[1];
      } else {
        *ex = x_data[0];
        *idx = 1;
      }
    }
  } else {
    int k;
    if (!std::isnan(x_data[0])) {
      *idx = 1;
    } else {
      boolean_T exitg1;
      *idx = 0;
      k = 2;
      exitg1 = false;
      while ((!exitg1) && (k <= last)) {
        if (!std::isnan(x_data[k - 1])) {
          *idx = k;
          exitg1 = true;
        } else {
          k++;
        }
      }
    }
    if (*idx == 0) {
      *ex = x_data[0];
      *idx = 1;
    } else {
      int i;
      *ex = x_data[*idx - 1];
      i = *idx + 1;
      for (k = i; k <= last; k++) {
        double d;
        d = x_data[k - 1];
        if (*ex < d) {
          *ex = d;
          *idx = k;
        }
      }
    }
  }
}

//
//
void minimum(const double x_data[], const int x_size[2], double *ex, int *idx)
{
  int last;
  last = x_size[1];
  if (x_size[1] <= 2) {
    if (x_size[1] == 1) {
      *ex = x_data[0];
      *idx = 1;
    } else {
      *ex = x_data[x_size[1] - 1];
      if ((x_data[0] > *ex) || (std::isnan(x_data[0]) && (!std::isnan(*ex)))) {
        *idx = x_size[1];
      } else {
        *ex = x_data[0];
        *idx = 1;
      }
    }
  } else {
    int k;
    if (!std::isnan(x_data[0])) {
      *idx = 1;
    } else {
      boolean_T exitg1;
      *idx = 0;
      k = 2;
      exitg1 = false;
      while ((!exitg1) && (k <= last)) {
        if (!std::isnan(x_data[k - 1])) {
          *idx = k;
          exitg1 = true;
        } else {
          k++;
        }
      }
    }
    if (*idx == 0) {
      *ex = x_data[0];
      *idx = 1;
    } else {
      int i;
      *ex = x_data[*idx - 1];
      i = *idx + 1;
      for (k = i; k <= last; k++) {
        double d;
        d = x_data[k - 1];
        if (*ex > d) {
          *ex = d;
          *idx = k;
        }
      }
    }
  }
}

} // namespace internal
} // namespace coder

// End of code generation (minOrMax.cpp)
