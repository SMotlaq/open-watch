//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// polyfit.cpp
//
// Code generation for function 'polyfit'
//

// Include files
#include "polyfit.h"
#include "xnrm2.h"
#include <algorithm>
#include <cmath>

// Function Declarations
static int div_nde_s32_floor(int numerator);

static double rt_hypotd_snf(double u0, double u1);

// Function Definitions
static int div_nde_s32_floor(int numerator)
{
  int b_numerator;
  if ((numerator < 0) && (numerator % 65 != 0)) {
    b_numerator = -1;
  } else {
    b_numerator = 0;
  }
  return numerator / 65 + b_numerator;
}

static double rt_hypotd_snf(double u0, double u1)
{
  double a;
  double y;
  a = std::abs(u0);
  y = std::abs(u1);
  if (a < y) {
    a /= y;
    y *= std::sqrt(a * a + 1.0);
  } else if (a > y) {
    y /= a;
    y = a * std::sqrt(y * y + 1.0);
  } else if (!std::isnan(y)) {
    y = a * 1.4142135623730951;
  }
  return y;
}

//
//
namespace coder {
void polyfit(const double y_data[], double p[2])
{
  static const double b_A[130]{0.0,
                               0.0075000000000000015,
                               0.015000000000000003,
                               0.022500000000000006,
                               0.030000000000000006,
                               0.037500000000000006,
                               0.045000000000000012,
                               0.052500000000000012,
                               0.060000000000000012,
                               0.067500000000000018,
                               0.075000000000000011,
                               0.082500000000000018,
                               0.090000000000000024,
                               0.097500000000000017,
                               0.10500000000000002,
                               0.11250000000000002,
                               0.12000000000000002,
                               0.12750000000000003,
                               0.13500000000000004,
                               0.14250000000000002,
                               0.15000000000000002,
                               0.15750000000000003,
                               0.16500000000000004,
                               0.17250000000000004,
                               0.18000000000000005,
                               0.18750000000000003,
                               0.19500000000000003,
                               0.20250000000000004,
                               0.21000000000000005,
                               0.21750000000000005,
                               0.22500000000000003,
                               0.23250000000000004,
                               0.24000000000000005,
                               0.24750000000000005,
                               0.25500000000000006,
                               0.26250000000000007,
                               0.27,
                               0.27750000000000008,
                               0.28500000000000003,
                               0.29250000000000009,
                               0.30000000000000004,
                               0.30750000000000005,
                               0.31500000000000006,
                               0.32250000000000006,
                               0.33000000000000007,
                               0.33750000000000008,
                               0.34500000000000008,
                               0.35250000000000004,
                               0.3600000000000001,
                               0.36750000000000005,
                               0.37500000000000006,
                               0.38250000000000006,
                               0.39000000000000007,
                               0.39750000000000008,
                               0.40500000000000008,
                               0.41250000000000009,
                               0.4200000000000001,
                               0.4275000000000001,
                               0.43500000000000005,
                               0.44250000000000012,
                               0.45000000000000007,
                               0.45750000000000007,
                               0.46500000000000008,
                               0.47250000000000009,
                               0.48000000000000009,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0,
                               1.0};
  static const double dv[130]{0.0,
                              0.0075000000000000015,
                              0.015000000000000003,
                              0.022500000000000006,
                              0.030000000000000006,
                              0.037500000000000006,
                              0.045000000000000012,
                              0.052500000000000012,
                              0.060000000000000012,
                              0.067500000000000018,
                              0.075000000000000011,
                              0.082500000000000018,
                              0.090000000000000024,
                              0.097500000000000017,
                              0.10500000000000002,
                              0.11250000000000002,
                              0.12000000000000002,
                              0.12750000000000003,
                              0.13500000000000004,
                              0.14250000000000002,
                              0.15000000000000002,
                              0.15750000000000003,
                              0.16500000000000004,
                              0.17250000000000004,
                              0.18000000000000005,
                              0.18750000000000003,
                              0.19500000000000003,
                              0.20250000000000004,
                              0.21000000000000005,
                              0.21750000000000005,
                              0.22500000000000003,
                              0.23250000000000004,
                              0.24000000000000005,
                              0.24750000000000005,
                              0.25500000000000006,
                              0.26250000000000007,
                              0.27,
                              0.27750000000000008,
                              0.28500000000000003,
                              0.29250000000000009,
                              0.30000000000000004,
                              0.30750000000000005,
                              0.31500000000000006,
                              0.32250000000000006,
                              0.33000000000000007,
                              0.33750000000000008,
                              0.34500000000000008,
                              0.35250000000000004,
                              0.3600000000000001,
                              0.36750000000000005,
                              0.37500000000000006,
                              0.38250000000000006,
                              0.39000000000000007,
                              0.39750000000000008,
                              0.40500000000000008,
                              0.41250000000000009,
                              0.4200000000000001,
                              0.4275000000000001,
                              0.43500000000000005,
                              0.44250000000000012,
                              0.45000000000000007,
                              0.45750000000000007,
                              0.46500000000000008,
                              0.47250000000000009,
                              0.48000000000000009,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0,
                              1.0};
  double A[130];
  double B_data[65];
  double tau[2];
  double vn1[2];
  double vn2[2];
  double work[2];
  double d;
  double scale;
  double t;
  double temp;
  int b_i;
  int ix0;
  int iy;
  int kend;
  signed char jpvt[2];
  std::copy(&y_data[0], &y_data[65], &B_data[0]);
  jpvt[0] = 1;
  tau[0] = 0.0;
  jpvt[1] = 2;
  tau[1] = 0.0;
  std::copy(&b_A[0], &b_A[130], &A[0]);
  for (int j{0}; j < 2; j++) {
    work[j] = 0.0;
    ix0 = j * 65;
    temp = 0.0;
    scale = 3.3121686421112381E-170;
    kend = ix0 + 65;
    for (int k{ix0 + 1}; k <= kend; k++) {
      d = dv[k - 1];
      if (d > scale) {
        t = scale / d;
        temp = temp * t * t + 1.0;
        scale = d;
      } else {
        t = d / scale;
        temp += t * t;
      }
    }
    d = scale * std::sqrt(temp);
    vn1[j] = d;
    vn2[j] = d;
  }
  for (int i{0}; i < 2; i++) {
    int ii;
    int ip1;
    int lastc;
    int pvt;
    ip1 = i + 2;
    ii = i * 65 + i;
    kend = 0;
    if ((2 - i > 1) && (std::abs(vn1[1]) > std::abs(vn1[i]))) {
      kend = 1;
    }
    pvt = i + kend;
    if (pvt != i) {
      kend = pvt * 65;
      iy = i * 65;
      for (int k{0}; k < 65; k++) {
        ix0 = kend + k;
        temp = A[ix0];
        lastc = iy + k;
        A[ix0] = A[lastc];
        A[lastc] = temp;
      }
      kend = jpvt[pvt];
      jpvt[pvt] = jpvt[i];
      jpvt[i] = static_cast<signed char>(kend);
      vn1[pvt] = vn1[i];
      vn2[pvt] = vn2[i];
    }
    t = A[ii];
    ix0 = ii + 2;
    tau[i] = 0.0;
    temp = internal::blas::xnrm2(64 - i, A, ii + 2);
    if (temp != 0.0) {
      d = A[ii];
      scale = rt_hypotd_snf(d, temp);
      if (d >= 0.0) {
        scale = -scale;
      }
      if (std::abs(scale) < 1.0020841800044864E-292) {
        kend = 0;
        b_i = (ii - i) + 65;
        do {
          kend++;
          for (int k{ix0}; k <= b_i; k++) {
            A[k - 1] *= 9.9792015476736E+291;
          }
          scale *= 9.9792015476736E+291;
          t *= 9.9792015476736E+291;
        } while ((std::abs(scale) < 1.0020841800044864E-292) && (kend < 20));
        scale = rt_hypotd_snf(t, internal::blas::xnrm2(64 - i, A, ii + 2));
        if (t >= 0.0) {
          scale = -scale;
        }
        tau[i] = (scale - t) / scale;
        temp = 1.0 / (t - scale);
        for (int k{ix0}; k <= b_i; k++) {
          A[k - 1] *= temp;
        }
        for (int k{0}; k < kend; k++) {
          scale *= 1.0020841800044864E-292;
        }
        t = scale;
      } else {
        tau[i] = (scale - d) / scale;
        temp = 1.0 / (d - scale);
        b_i = (ii - i) + 65;
        for (int k{ix0}; k <= b_i; k++) {
          A[k - 1] *= temp;
        }
        t = scale;
      }
    }
    A[ii] = t;
    if (i + 1 < 2) {
      A[ii] = 1.0;
      ix0 = ii + 66;
      if (tau[0] != 0.0) {
        pvt = 65;
        kend = ii + 64;
        while ((pvt > 0) && (A[kend] == 0.0)) {
          pvt--;
          kend--;
        }
        lastc = 1;
        kend = ii + 65;
        int exitg1;
        do {
          exitg1 = 0;
          if (kend + 1 <= (ii + pvt) + 65) {
            if (A[kend] != 0.0) {
              exitg1 = 1;
            } else {
              kend++;
            }
          } else {
            lastc = 0;
            exitg1 = 1;
          }
        } while (exitg1 == 0);
      } else {
        pvt = 0;
        lastc = 0;
      }
      if (pvt > 0) {
        if (lastc != 0) {
          work[0] = 0.0;
          for (iy = ix0; iy <= ix0; iy += 65) {
            temp = 0.0;
            b_i = (iy + pvt) - 1;
            for (kend = iy; kend <= b_i; kend++) {
              temp += A[kend - 1] * A[(ii + kend) - iy];
            }
            kend = div_nde_s32_floor((iy - ii) - 66);
            work[kend] += temp;
          }
        }
        if (!(-tau[0] == 0.0)) {
          kend = ii;
          for (int j{0}; j < lastc; j++) {
            if (work[0] != 0.0) {
              temp = work[0] * -tau[0];
              b_i = kend + 66;
              ix0 = pvt + kend;
              for (iy = b_i; iy <= ix0 + 65; iy++) {
                A[iy - 1] += A[((ii + iy) - kend) - 66] * temp;
              }
            }
            kend += 65;
          }
        }
      }
      A[ii] = t;
    }
    for (int j{ip1}; j < 3; j++) {
      if (vn1[1] != 0.0) {
        temp = std::abs(A[i + 65]) / vn1[1];
        temp = 1.0 - temp * temp;
        if (temp < 0.0) {
          temp = 0.0;
        }
        scale = vn1[1] / vn2[1];
        scale = temp * (scale * scale);
        if (scale <= 1.4901161193847656E-8) {
          d = internal::blas::xnrm2(64 - i, A, i + 67);
          vn1[1] = d;
          vn2[1] = d;
        } else {
          vn1[1] *= std::sqrt(temp);
        }
      }
    }
  }
  iy = 0;
  for (int k{0}; k < 2; k++) {
    if (A[k + 65 * k] != 0.0) {
      iy++;
    }
    p[k] = 0.0;
    if (tau[k] != 0.0) {
      temp = B_data[k];
      b_i = k + 2;
      for (int i{b_i}; i < 66; i++) {
        temp += A[(i + 65 * k) - 1] * B_data[i - 1];
      }
      temp *= tau[k];
      if (temp != 0.0) {
        B_data[k] -= temp;
        for (int i{b_i}; i < 66; i++) {
          B_data[i - 1] -= A[(i + 65 * k) - 1] * temp;
        }
      }
    }
  }
  for (int i{0}; i < iy; i++) {
    p[jpvt[i] - 1] = B_data[i];
  }
  for (int j{iy}; j >= 1; j--) {
    kend = jpvt[j - 1] - 1;
    ix0 = 65 * (j - 1);
    p[kend] /= A[(j + ix0) - 1];
    for (int i{0}; i <= j - 2; i++) {
      p[jpvt[0] - 1] -= p[kend] * A[ix0];
    }
  }
}

} // namespace coder

// End of code generation (polyfit.cpp)
