//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// filtfilt.cpp
//
// Code generation for function 'filtfilt'
//

// Include files
#include "filtfilt.h"
#include "PPG_Algorithm_data.h"
#include "filter.h"
#include "introsort.h"
#include "minOrMax.h"
#include "sparse.h"
#include "coder_array.h"
#include "cs.h"
#include "makeCXSparseMatrix.h"
#include "solve_from_lu.h"
#include "solve_from_qr.h"
#include <algorithm>
#include <cmath>
#include <cstring>

// Function Declarations
static int div_s32(int numerator, int denominator);

// Function Definitions
static int div_s32(int numerator, int denominator)
{
  int quotient;
  if (denominator == 0) {
    if (numerator >= 0) {
      quotient = MAX_int32_T;
    } else {
      quotient = MIN_int32_T;
    }
  } else {
    unsigned int b_denominator;
    unsigned int b_numerator;
    if (numerator < 0) {
      b_numerator = ~static_cast<unsigned int>(numerator) + 1U;
    } else {
      b_numerator = static_cast<unsigned int>(numerator);
    }
    if (denominator < 0) {
      b_denominator = ~static_cast<unsigned int>(denominator) + 1U;
    } else {
      b_denominator = static_cast<unsigned int>(denominator);
    }
    b_numerator /= b_denominator;
    if ((numerator < 0) != (denominator < 0)) {
      quotient = -static_cast<int>(b_numerator);
    } else {
      quotient = static_cast<int>(b_numerator);
    }
  }
  return quotient;
}

//
//
namespace coder {
void filtfilt(const double x[1000], double y_data[], int y_size[2])
{
  static const double b[102]{
      -1.63796910266374E-5,  -2.84608791354145E-5,  -3.47948616986674E-5,
      -2.56727114087492E-5,  7.3839908593977E-6,    6.67680505647034E-5,
      0.000144220744648007,  0.000218650767704713,  0.000258280143077183,
      0.00022807263841626,   0.000101693300900881,  -0.000124758690006743,
      -0.000421650902575332, -0.000722336468397606, -0.000931249983544117,
      -0.000944804070397773, -0.000682323484488254, -0.000120026688940773,
      0.000682048689171401,  0.00157095511428104,   0.00231774241440107,
      0.00266262795919279,   0.00238256832825357,   0.00136701303484241,
      -0.000318457684967701, -0.0024023519489018,   -0.00442843906776125,
      -0.00583430818467503,  -0.00607890845092893,  -0.00479507258465654,
      -0.00193093675781346,  0.00216038155862557,   0.00671787852679799,
      0.0106755431378323,    0.0128661463672932,    0.0122943655106001,
      0.00842469157575729,   0.00141678148128293,   -0.00775700398531743,
      -0.0173587517112779,   -0.0251115584077625,   -0.0285805768241776,
      -0.0256452699671197,   -0.0149718649982291,   0.00361455682767386,
      0.0289464903889845,    0.0585739345317463,    0.0890750464869367,
      0.116572433748717,     0.13736469277032,      0.148554990916328,
      0.148554990916328,     0.13736469277032,      0.116572433748717,
      0.0890750464869367,    0.0585739345317463,    0.0289464903889845,
      0.00361455682767386,   -0.0149718649982291,   -0.0256452699671197,
      -0.0285805768241776,   -0.0251115584077625,   -0.0173587517112779,
      -0.00775700398531743,  0.00141678148128293,   0.00842469157575729,
      0.0122943655106001,    0.0128661463672932,    0.0106755431378323,
      0.00671787852679799,   0.00216038155862557,   -0.00193093675781346,
      -0.00479507258465654,  -0.00607890845092893,  -0.00583430818467503,
      -0.00442843906776125,  -0.0024023519489018,   -0.000318457684967701,
      0.00136701303484241,   0.00238256832825357,   0.00266262795919279,
      0.00231774241440107,   0.00157095511428104,   0.000682048689171401,
      -0.000120026688940773, -0.000682323484488254, -0.000944804070397773,
      -0.000931249983544117, -0.000722336468397606, -0.000421650902575332,
      -0.000124758690006743, 0.000101693300900881,  0.00022807263841626,
      0.000258280143077183,  0.000218650767704713,  0.000144220744648007,
      6.67680505647034E-5,   7.3839908593977E-6,    -2.56727114087492E-5,
      -3.47948616986674E-5,  -2.84608791354145E-5,  -1.63796910266374E-5};
  cs_din *N;
  cs_dis *S;
  sparse y;
  double b_ytemp_data[1606];
  double ytemp_data[1606];
  double b2_data[102];
  double b_a2_data[102];
  double b_b2_data[102];
  double outBuff[101];
  double outBuff_data[101];
  double d;
  double val;
  int sortedIndices[301];
  int cptr;
  int i;
  signed char cidxInt[301];
  signed char ridxInt[301];
  signed char vals[301];
  signed char a2_data[102];
  std::copy(&b[0], &b[102], &b2_data[0]);
  a2_data[0] = 1;
  std::memset(&a2_data[1], 0, 101U * sizeof(signed char));
  vals[0] = static_cast<signed char>(a2_data[1] + 1);
  for (i = 0; i < 100; i++) {
    vals[i + 1] = a2_data[i + 2];
    vals[i + 101] = 1;
    vals[i + 201] = -1;
  }
  for (i = 0; i < 101; i++) {
    outBuff[i] =
        b2_data[i + 1] - b2_data[0] * static_cast<double>(a2_data[i + 1]);
  }
  for (cptr = 0; cptr < 301; cptr++) {
    sortedIndices[cptr] = cptr + 1;
  }
  internal::introsort(sortedIndices);
  y.d.set_size(301);
  y.colidx.set_size(102);
  y.colidx[0] = 1;
  y.rowidx.set_size(301);
  for (cptr = 0; cptr < 301; cptr++) {
    i = sortedIndices[cptr];
    cidxInt[cptr] = iv[i - 1];
    ridxInt[cptr] = iv1[i - 1];
    y.d[cptr] = 0.0;
    y.rowidx[cptr] = 0;
  }
  cptr = 0;
  for (int c{0}; c < 101; c++) {
    while ((cptr + 1 <= 301) && (cidxInt[cptr] == c + 1)) {
      y.rowidx[cptr] = ridxInt[cptr];
      cptr++;
    }
    y.colidx[c + 1] = cptr + 1;
  }
  for (cptr = 0; cptr < 301; cptr++) {
    y.d[cptr] = vals[sortedIndices[cptr] - 1];
  }
  cptr = 1;
  for (int c{0}; c < 101; c++) {
    int ridx;
    ridx = y.colidx[c];
    y.colidx[c] = cptr;
    int exitg1;
    do {
      exitg1 = 0;
      i = y.colidx[c + 1];
      if (ridx < i) {
        int currRowIdx;
        val = 0.0;
        currRowIdx = y.rowidx[ridx - 1];
        while ((ridx < i) && (y.rowidx[ridx - 1] == currRowIdx)) {
          val += y.d[ridx - 1];
          ridx++;
        }
        if (val != 0.0) {
          y.d[cptr - 1] = val;
          y.rowidx[cptr - 1] = currRowIdx;
          cptr++;
        }
      } else {
        exitg1 = 1;
      }
    } while (exitg1 == 0);
  }
  cs_di *cxA;
  y.colidx[101] = cptr;
  cxA = makeCXSparseMatrix(y.colidx[101] - 1, 101, 101, &(y.colidx.data())[0],
                           &(y.rowidx.data())[0], &(y.d.data())[0]);
  S = cs_di_sqr(2, cxA, 0);
  N = cs_di_lu(cxA, S, 1);
  cs_di_spfree(cxA);
  if (N == nullptr) {
    cs_di *b_cxA;
    cs_din *b_N;
    cs_dis *b_S;
    cs_di_sfree(S);
    cs_di_nfree(N);
    b_cxA =
        makeCXSparseMatrix(y.colidx[101] - 1, 101, 101, &(y.colidx.data())[0],
                           &(y.rowidx.data())[0], &(y.d.data())[0]);
    b_S = cs_di_sqr(2, b_cxA, 1);
    b_N = cs_di_qr(b_cxA, b_S);
    cs_di_spfree(b_cxA);
    qr_rank_di(b_N, &val);
    solve_from_qr_di(b_N, b_S, (double *)&outBuff[0], 101, 101);
    cs_di_sfree(b_S);
    cs_di_nfree(b_N);
  } else {
    solve_from_lu_di(N, S, (double *)&outBuff[0], 101);
    cs_di_sfree(S);
    cs_di_nfree(N);
  }
  val = 2.0 * x[0];
  d = 2.0 * x[999];
  for (i = 0; i < 303; i++) {
    ytemp_data[i] = val - x[303 - i];
  }
  std::copy(&x[0], &x[1000], &ytemp_data[303]);
  for (i = 0; i < 303; i++) {
    ytemp_data[i + 1303] = d - x[998 - i];
  }
  for (i = 0; i < 102; i++) {
    b_b2_data[i] = b2_data[i];
    b_a2_data[i] = a2_data[i];
  }
  std::copy(&ytemp_data[0], &ytemp_data[1606], &b_ytemp_data[0]);
  for (i = 0; i < 101; i++) {
    outBuff_data[i] = outBuff[i] * ytemp_data[0];
  }
  filter(b_b2_data, b_a2_data, b_ytemp_data, outBuff_data, ytemp_data, &cptr);
  for (i = 0; i < 1606; i++) {
    b_ytemp_data[i] = ytemp_data[1605 - i];
  }
  std::copy(&b_ytemp_data[0], &b_ytemp_data[1606], &ytemp_data[0]);
  for (i = 0; i < 102; i++) {
    b_a2_data[i] = a2_data[i];
  }
  std::copy(&ytemp_data[0], &ytemp_data[1606], &b_ytemp_data[0]);
  for (i = 0; i < 101; i++) {
    outBuff_data[i] = outBuff[i] * ytemp_data[0];
  }
  filter(b2_data, b_a2_data, b_ytemp_data, outBuff_data, ytemp_data, &cptr);
  y_size[0] = 1;
  y_size[1] = 1000;
  for (i = 0; i < 1000; i++) {
    y_data[i] = ytemp_data[1302 - i];
  }
}

//
//
void filtfilt(const double x_data[], ::coder::array<double, 2U> &y)
{
  static const double dv[30]{1.0,
                             1.0,
                             1.0,
                             1.0,
                             1.0,
                             -1.99991385258528,
                             -1.99993338029358,
                             -1.99996329884828,
                             -1.99998960915359,
                             -1.0,
                             1.0,
                             1.0,
                             1.0,
                             1.0,
                             0.0,
                             1.0,
                             1.0,
                             1.0,
                             1.0,
                             1.0,
                             -1.99673421340156,
                             -1.99095397327505,
                             -1.98628047859018,
                             -1.98324862235718,
                             -0.991099539970636,
                             0.996900026371429,
                             0.991099864595603,
                             0.986396311407408,
                             0.983338186771914,
                             0.0};
  static const double g[6]{0.998430062985425, 0.995530039947411,
                           0.993178310196357, 0.99164927830111,
                           0.995549769985318, 1.0};
  array<double, 1U> yCol;
  array<double, 1U> ytemp;
  double b[30];
  double a2_data[15];
  double b2_data[15];
  double a1_data[11];
  double a2[11];
  double b1_data[11];
  double b2[11];
  double varargin_1_data[11];
  double zi[10];
  double A_data[9];
  double A[4];
  double rhs[2];
  double a2_data_tmp;
  double b2_data_tmp;
  double b_a2_data_tmp;
  double b_b2_data_tmp;
  double maxCoefNum;
  int varargin_1_size[2];
  int b1_tmp;
  int idx;
  int k;
  int loop_ub_tmp;
  int nA_tmp;
  int na;
  int nb;
  int r1;
  signed char ii_data;
  signed char nZeroLastDen_data;
  boolean_T b_x_data[11];
  boolean_T exitg1;
  std::copy(&dv[0], &dv[30], &b[0]);
  b[4] = 1.0;
  b[9] = -1.0;
  b[14] = 0.0;
  nb = 11;
  na = 11;
  b2[0] = 1.0;
  a2[0] = 1.0;
  for (int ii{0}; ii < 5; ii++) {
    a2_data_tmp = g[ii];
    b[ii] *= a2_data_tmp;
    b[ii + 5] *= a2_data_tmp;
    b[ii + 10] *= a2_data_tmp;
    loop_ub_tmp = (ii + 1) << 1;
    idx = loop_ub_tmp - 1;
    std::copy(&b2[0], &b2[idx], &A_data[0]);
    nA_tmp = loop_ub_tmp - 2;
    r1 = loop_ub_tmp + 1;
    std::memset(&b1_data[0], 0, r1 * sizeof(double));
    if (loop_ub_tmp - 1 < 3) {
      for (int b_k{0}; b_k <= nA_tmp; b_k++) {
        a2_data_tmp = A_data[b_k];
        b1_data[b_k] += a2_data_tmp * b[ii];
        b1_data[b_k + 1] += a2_data_tmp * b[ii + 5];
        b1_data[b_k + 2] += a2_data_tmp * b[ii + 10];
      }
    } else {
      for (int b_k{0}; b_k < 3; b_k++) {
        for (k = 0; k <= nA_tmp; k++) {
          b1_tmp = b_k + k;
          b1_data[b1_tmp] += b[ii + 5 * b_k] * A_data[k];
        }
      }
    }
    std::copy(&b1_data[0], &b1_data[r1], &b2[0]);
    std::copy(&a2[0], &a2[idx], &A_data[0]);
    std::memset(&b1_data[0], 0, r1 * sizeof(double));
    if (loop_ub_tmp - 1 < 3) {
      for (int b_k{0}; b_k <= nA_tmp; b_k++) {
        a2_data_tmp = A_data[b_k];
        b1_data[b_k] += a2_data_tmp * b[ii + 15];
        b1_data[b_k + 1] += a2_data_tmp * b[ii + 20];
        b1_data[b_k + 2] += a2_data_tmp * b[ii + 25];
      }
    } else {
      for (int b_k{0}; b_k < 3; b_k++) {
        for (k = 0; k <= nA_tmp; k++) {
          b1_tmp = b_k + k;
          b1_data[b1_tmp] += b[ii + 5 * (b_k + 3)] * A_data[k];
        }
      }
    }
    std::copy(&b1_data[0], &b1_data[r1], &a2[0]);
  }
  if (b2[10] == 0.0) {
    nb = 10;
  }
  if (a2[10] == 0.0) {
    na = 10;
  }
  std::copy(&b2[0], &b2[nb], &b1_data[0]);
  std::copy(&a2[0], &a2[na], &a1_data[0]);
  k = nb - 1;
  varargin_1_size[0] = 1;
  varargin_1_size[1] = nb;
  for (int b_k{0}; b_k <= k; b_k++) {
    varargin_1_data[b_k] = std::abs(b1_data[b_k]);
  }
  maxCoefNum = internal::maximum(varargin_1_data, varargin_1_size);
  if (maxCoefNum != 0.0) {
    for (k = 0; k < nb; k++) {
      b1_data[k] = b2[k] / maxCoefNum;
    }
  }
  k = na - 1;
  varargin_1_size[0] = 1;
  varargin_1_size[1] = na;
  for (int b_k{0}; b_k <= k; b_k++) {
    varargin_1_data[b_k] = std::abs(a1_data[b_k]);
  }
  maxCoefNum = internal::maximum(varargin_1_data, varargin_1_size);
  if (maxCoefNum != 0.0) {
    for (k = 0; k < na; k++) {
      a1_data[k] = a2[k] / maxCoefNum;
    }
  }
  for (k = 0; k < nb; k++) {
    b_x_data[k] = (b1_data[k] != 0.0);
  }
  idx = 0;
  nA_tmp = 1;
  exitg1 = false;
  while ((!exitg1) && (nb > 0)) {
    if (b_x_data[nb - 1]) {
      idx = 1;
      ii_data = static_cast<signed char>(nb);
      exitg1 = true;
    } else {
      nb--;
    }
  }
  if (idx == 0) {
    nA_tmp = 0;
  }
  r1 = nA_tmp;
  for (k = 0; k < nA_tmp; k++) {
    nZeroLastDen_data = ii_data;
  }
  for (k = 0; k < na; k++) {
    b_x_data[k] = (a1_data[k] != 0.0);
  }
  idx = 0;
  nA_tmp = 1;
  exitg1 = false;
  while ((!exitg1) && (na > 0)) {
    if (b_x_data[na - 1]) {
      idx = 1;
      ii_data = static_cast<signed char>(na);
      exitg1 = true;
    } else {
      na--;
    }
  }
  if (idx == 0) {
    nA_tmp = 0;
  }
  if (r1 == 0) {
    nZeroLastDen_data = 0;
  }
  if (nA_tmp == 0) {
    ii_data = 0;
  }
  b1_tmp = static_cast<int>(
      std::fmax(1.0, 3.0 * (std::fmax(static_cast<double>(ii_data),
                                      static_cast<double>(nZeroLastDen_data)) -
                            1.0)));
  A[2] = -1.0;
  A[3] = 1.0;
  for (int ii{0}; ii < 5; ii++) {
    maxCoefNum = b[ii + 15];
    a2_data[3 * ii] = maxCoefNum / maxCoefNum;
    a2_data_tmp = b[ii] / maxCoefNum;
    b2_data[3 * ii] = a2_data_tmp;
    b2_data_tmp = b[ii + 20] / maxCoefNum;
    idx = 3 * ii + 1;
    a2_data[idx] = b2_data_tmp;
    b_a2_data_tmp = b[ii + 5] / maxCoefNum;
    b2_data[idx] = b_a2_data_tmp;
    b_b2_data_tmp = b[ii + 25] / maxCoefNum;
    idx = 3 * ii + 2;
    a2_data[idx] = b_b2_data_tmp;
    maxCoefNum = b[ii + 10] / maxCoefNum;
    b2_data[idx] = maxCoefNum;
    rhs[0] = b_a2_data_tmp - a2_data_tmp * b2_data_tmp;
    rhs[1] = maxCoefNum - a2_data_tmp * b_b2_data_tmp;
    A[0] = 1.0 - (-b2_data_tmp);
    A[1] = 0.0 - (-b_b2_data_tmp);
    if (std::abs(0.0 - (-b_b2_data_tmp)) > std::abs(1.0 - (-b2_data_tmp))) {
      r1 = 1;
      idx = 0;
    } else {
      r1 = 0;
      idx = 1;
    }
    maxCoefNum = A[idx] / A[r1];
    k = static_cast<int>(A[r1 + 2]);
    a2_data_tmp = (rhs[idx] - rhs[r1] * maxCoefNum) /
                  (A[idx + 2] - maxCoefNum * static_cast<double>(k));
    idx = ii << 1;
    zi[idx + 1] = a2_data_tmp;
    zi[idx] = (rhs[r1] - a2_data_tmp * static_cast<double>(k)) / A[r1];
  }
  yCol.set_size(1000);
  for (k = 0; k < 1000; k++) {
    yCol[k] = x_data[k];
  }
  loop_ub_tmp = b1_tmp - 1;
  for (int ii{0}; ii < 5; ii++) {
    double b_a2_data[3];
    double b_b2_data[3];
    double b_rhs_tmp;
    double c_a2_data_tmp;
    double rhs_tmp;
    a2_data_tmp = 2.0 * yCol[0];
    b2_data_tmp = 2.0 * yCol[yCol.size(0) - 1];
    idx = yCol.size(0);
    ytemp.set_size((b1_tmp + yCol.size(0)) + b1_tmp);
    for (k = 0; k <= loop_ub_tmp; k++) {
      ytemp[k] = a2_data_tmp - yCol[b1_tmp - k];
    }
    for (k = 0; k < idx; k++) {
      ytemp[k + b1_tmp] = yCol[k];
    }
    for (k = 0; k <= loop_ub_tmp; k++) {
      ytemp[(k + b1_tmp) + idx] = b2_data_tmp - yCol[(yCol.size(0) - k) - 2];
    }
    nA_tmp = ytemp.size(0) - 1;
    maxCoefNum = b2_data[3 * ii];
    b_b2_data[0] = maxCoefNum;
    a2_data_tmp = a2_data[3 * ii];
    b_a2_data[0] = a2_data_tmp;
    idx = 3 * ii + 1;
    b2_data_tmp = b2_data[idx];
    b_b2_data[1] = b2_data_tmp;
    b_a2_data_tmp = a2_data[idx];
    b_a2_data[1] = b_a2_data_tmp;
    idx = 3 * ii + 2;
    b_b2_data_tmp = b2_data[idx];
    b_b2_data[2] = b_b2_data_tmp;
    c_a2_data_tmp = a2_data[idx];
    b_a2_data[2] = c_a2_data_tmp;
    yCol.set_size(ytemp.size(0));
    for (k = 0; k <= nA_tmp; k++) {
      yCol[k] = ytemp[k];
    }
    idx = ii << 1;
    rhs_tmp = zi[idx];
    rhs[0] = rhs_tmp * ytemp[0];
    b_rhs_tmp = zi[idx + 1];
    rhs[1] = b_rhs_tmp * ytemp[0];
    filter(b_b2_data, b_a2_data, yCol, rhs, ytemp);
    if (ytemp.size(0) < 1) {
      k = 0;
      nA_tmp = 1;
      r1 = -1;
    } else {
      k = ytemp.size(0) - 1;
      nA_tmp = -1;
      r1 = 0;
    }
    idx = div_s32(r1 - k, nA_tmp);
    yCol.set_size(idx + 1);
    for (r1 = 0; r1 <= idx; r1++) {
      yCol[r1] = ytemp[k + nA_tmp * r1];
    }
    ytemp.set_size(yCol.size(0));
    idx = yCol.size(0);
    for (k = 0; k < idx; k++) {
      ytemp[k] = yCol[k];
    }
    nA_tmp = ytemp.size(0) - 1;
    b_b2_data[0] = maxCoefNum;
    b_a2_data[0] = a2_data_tmp;
    b_b2_data[1] = b2_data_tmp;
    b_a2_data[1] = b_a2_data_tmp;
    b_b2_data[2] = b_b2_data_tmp;
    b_a2_data[2] = c_a2_data_tmp;
    yCol.set_size(ytemp.size(0));
    for (k = 0; k <= nA_tmp; k++) {
      yCol[k] = ytemp[k];
    }
    rhs[0] = rhs_tmp * ytemp[0];
    rhs[1] = b_rhs_tmp * ytemp[0];
    filter(b_b2_data, b_a2_data, yCol, rhs, ytemp);
    k = ytemp.size(0) - b1_tmp;
    if (b1_tmp + 1 > k) {
      k = 0;
      nA_tmp = 1;
      r1 = -1;
    } else {
      k--;
      nA_tmp = -1;
      r1 = b1_tmp;
    }
    idx = div_s32(r1 - k, nA_tmp);
    yCol.set_size(idx + 1);
    for (r1 = 0; r1 <= idx; r1++) {
      yCol[r1] = ytemp[k + nA_tmp * r1];
    }
  }
  y.set_size(1, yCol.size(0));
  idx = yCol.size(0);
  for (k = 0; k < idx; k++) {
    y[k] = yCol[k];
  }
}

} // namespace coder

// End of code generation (filtfilt.cpp)
