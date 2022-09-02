//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// ceil.cpp
//
// Code generation for function 'ceil'
//

// Include files
#include "ceil.h"
#include "mean.h"
#include <cmath>

// Function Definitions
double binary_expand_op(const double in1[1000], const double in2[31], int in5,
                        int in6, const double in7[31], int in10, int in11,
                        double in12, const double in13[1000],
                        const double in14_data[], const int in14_size[2],
                        const double in15_data[], const int in15_size[2],
                        const double in16[1000])
{
  double b_in14_data[31];
  double in1_data[31];
  int b_in14_size[2];
  int in1_size[2];
  int loop_ub;
  int stride_0_1;
  int stride_1_1;
  in1_size[0] = 1;
  if ((in11 - in10) + 1 == 1) {
    in1_size[1] = (in6 - in5) + 1;
  } else {
    in1_size[1] = (in11 - in10) + 1;
  }
  stride_0_1 = ((in6 - in5) + 1 != 1);
  stride_1_1 = ((in11 - in10) + 1 != 1);
  if ((in11 - in10) + 1 == 1) {
    loop_ub = (in6 - in5) + 1;
  } else {
    loop_ub = (in11 - in10) + 1;
  }
  for (int i{0}; i < loop_ub; i++) {
    in1_data[i] = in1[static_cast<int>(in2[in5 + i * stride_0_1]) - 1] -
                  in1[static_cast<int>(in7[in10 + i * stride_1_1]) - 1] * in12;
  }
  b_in14_size[0] = 1;
  if (in15_size[1] == 1) {
    b_in14_size[1] = in14_size[1];
  } else {
    b_in14_size[1] = in15_size[1];
  }
  stride_0_1 = (in14_size[1] != 1);
  stride_1_1 = (in15_size[1] != 1);
  if (in15_size[1] == 1) {
    loop_ub = in14_size[1];
  } else {
    loop_ub = in15_size[1];
  }
  for (int i{0}; i < loop_ub; i++) {
    b_in14_data[i] = in14_data[i * stride_0_1] - in15_data[i * stride_1_1];
  }
  return std::ceil(
      (104.0 - 28.0 * (std::abs(coder::mean(in1_data, in1_size) /
                                std::abs(coder::mean(in13))) /
                       std::abs(coder::mean(b_in14_data, b_in14_size) /
                                std::abs(coder::mean(in16))))) *
      7.0);
}

// End of code generation (ceil.cpp)
