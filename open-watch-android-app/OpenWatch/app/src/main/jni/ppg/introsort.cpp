//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// introsort.cpp
//
// Code generation for function 'introsort'
//

// Include files
#include "introsort.h"
#include "PPG_Algorithm_data.h"
#include "PPG_Algorithm_internal_types.h"
#include "heapsort.h"
#include "insertionsort.h"
#include "stack1.h"

// Function Definitions
//
//
namespace coder {
namespace internal {
void introsort(int x[301])
{
  stack st;
  st.init();
  st.d[0].xstart = 1;
  st.d[0].xend = 301;
  st.d[0].depth = 0;
  st.n = 1;
  while (st.n > 0) {
    struct_T expl_temp;
    int expl_temp_tmp_tmp;
    int s_depth;
    int t;
    expl_temp_tmp_tmp = st.n - 1;
    expl_temp = st.d[st.n - 1];
    s_depth = st.d[st.n - 1].depth;
    st.n--;
    t = expl_temp.xend - expl_temp.xstart;
    if (t + 1 <= 32) {
      insertionsort(x, expl_temp.xstart, expl_temp.xend);
    } else if (expl_temp.depth == 16) {
      b_heapsort(x, expl_temp.xstart, expl_temp.xend);
    } else {
      int ai;
      int aj;
      int aj_tmp;
      int j;
      int pivot;
      int xmid;
      boolean_T varargout_1;
      xmid = (expl_temp.xstart + t / 2) - 1;
      ai = iv[x[xmid] - 1];
      t = x[expl_temp.xstart - 1];
      aj = iv[t - 1];
      if (ai < aj) {
        varargout_1 = true;
      } else if (ai == aj) {
        varargout_1 = (iv1[x[xmid] - 1] < iv1[t - 1]);
      } else {
        varargout_1 = false;
      }
      if (varargout_1) {
        x[expl_temp.xstart - 1] = x[xmid];
        x[xmid] = t;
      }
      ai = iv[x[expl_temp.xend - 1] - 1];
      t = x[expl_temp.xstart - 1];
      aj = iv[t - 1];
      if (ai < aj) {
        varargout_1 = true;
      } else if (ai == aj) {
        varargout_1 = (iv1[x[expl_temp.xend - 1] - 1] < iv1[t - 1]);
      } else {
        varargout_1 = false;
      }
      if (varargout_1) {
        x[expl_temp.xstart - 1] = x[expl_temp.xend - 1];
        x[expl_temp.xend - 1] = t;
      }
      ai = iv[x[expl_temp.xend - 1] - 1];
      aj = iv[x[xmid] - 1];
      if (ai < aj) {
        varargout_1 = true;
      } else if (ai == aj) {
        varargout_1 = (iv1[x[expl_temp.xend - 1] - 1] < iv1[x[xmid] - 1]);
      } else {
        varargout_1 = false;
      }
      if (varargout_1) {
        t = x[xmid];
        x[xmid] = x[expl_temp.xend - 1];
        x[expl_temp.xend - 1] = t;
      }
      pivot = x[xmid] - 1;
      x[xmid] = x[expl_temp.xend - 2];
      x[expl_temp.xend - 2] = pivot + 1;
      xmid = expl_temp.xstart - 1;
      j = expl_temp.xend - 2;
      aj_tmp = iv[pivot];
      int exitg1;
      do {
        int exitg2;
        exitg1 = 0;
        xmid++;
        do {
          exitg2 = 0;
          ai = iv[x[xmid] - 1];
          if (ai < aj_tmp) {
            varargout_1 = true;
          } else if (ai == aj_tmp) {
            varargout_1 = (iv1[x[xmid] - 1] < iv1[pivot]);
          } else {
            varargout_1 = false;
          }
          if (varargout_1) {
            xmid++;
          } else {
            exitg2 = 1;
          }
        } while (exitg2 == 0);
        j--;
        do {
          exitg2 = 0;
          aj = iv[x[j] - 1];
          if (aj_tmp < aj) {
            varargout_1 = true;
          } else if (aj_tmp == aj) {
            varargout_1 = (iv1[pivot] < iv1[x[j] - 1]);
          } else {
            varargout_1 = false;
          }
          if (varargout_1) {
            j--;
          } else {
            exitg2 = 1;
          }
        } while (exitg2 == 0);
        if (xmid + 1 >= j + 1) {
          exitg1 = 1;
        } else {
          t = x[xmid];
          x[xmid] = x[j];
          x[j] = t;
        }
      } while (exitg1 == 0);
      x[expl_temp.xend - 2] = x[xmid];
      x[xmid] = pivot + 1;
      if (xmid + 2 < expl_temp.xend) {
        st.d[expl_temp_tmp_tmp].xstart = xmid + 2;
        st.d[expl_temp_tmp_tmp].xend = expl_temp.xend;
        st.d[expl_temp_tmp_tmp].depth = s_depth + 1;
        st.n = expl_temp_tmp_tmp + 1;
      }
      if (expl_temp.xstart < xmid + 1) {
        st.d[st.n].xstart = expl_temp.xstart;
        st.d[st.n].xend = xmid + 1;
        st.d[st.n].depth = s_depth + 1;
        st.n++;
      }
    }
  }
}

} // namespace internal
} // namespace coder

// End of code generation (introsort.cpp)
