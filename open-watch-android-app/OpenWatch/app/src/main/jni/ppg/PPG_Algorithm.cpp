//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// PPG_Algorithm.cpp
//
// Code generation for function 'PPG_Algorithm'
//

// Include files
#include "PPG_Algorithm.h"
#include "PPG_Algorithm_data.h"
#include "PPG_Algorithm_initialize.h"
#include "ceil.h"
#include "diff.h"
#include "filtfilt.h"
#include "mean.h"
#include "minOrMax.h"
#include "nonzeros.h"
#include "polyfit.h"
#include "rand.h"
#include "coder_array.h"
#include <algorithm>
#include <cmath>

// Function Definitions
//
// function [HR, SpO2, PPG_IR, PPG_RED, index] = PPG_Algorithm(rawsig_IR,
// rawsig_RED, gender, age, SpO2_prev)
void PPG_Algorithm(const double rawsig_IR[1000], const double rawsig_RED[1000],
                   boolean_T gender, int age, double SpO2_prev, double *HR,
                   double *SpO2, double PPG_IR[1000], double PPG_RED[1000],
                   int *b_index)
{
  static const signed char b_iv[14]{-2, -1, -1, 0, 0, 0, 0,
                                    0,  0,  0,  0, 1, 1, 2};
  coder::array<double, 2U> sig_IR_filt2_buff;
  double sig_IR_filt1_buff_data[1000];
  double PPG_IR_data[999];
  double b_tmp_data[31];
  double c_tmp_data[31];
  double peak_indices[31];
  double peak_indices_data[31];
  double trough_indices[31];
  double b_y_data[30];
  double c_y_data[30];
  double d_y_data[30];
  double tmp_data[30];
  double y_data[30];
  double line[2];
  double Tamp_idx_0;
  double Tamp_idx_1;
  double amp;
  double current_grad;
  double max_sig_IR;
  double prev_peak;
  double prev_trough;
  int PPG_IR_size[2];
  int b_tmp_size[2];
  int c_tmp_size[2];
  int peak_indices_size[2];
  int sig_IR_filt1_buff_size[2];
  int tmp_size[2];
  int b_i;
  int b_loop_ub;
  int b_y_size;
  int c_loop_ub;
  int c_y_size;
  int d_y_size;
  int i;
  int iindx;
  int loop_ub;
  int peak_indices_counter;
  int trough_indices_counter;
  int y_size;
  boolean_T flag;
  if (!isInitialized_PPG_Algorithm) {
    PPG_Algorithm_initialize();
  }
  // 'PPG_Algorithm:2' LPF_FIR_coeff =
  // [-1.63796910266374e-05,-2.84608791354145e-05,-3.47948616986674e-05,-2.56727114087492e-05,7.38399085939770e-06,6.67680505647034e-05,0.000144220744648007,0.000218650767704713,0.000258280143077183,0.000228072638416260,0.000101693300900881,-0.000124758690006743,-0.000421650902575332,-0.000722336468397606,-0.000931249983544117,-0.000944804070397773,-0.000682323484488254,-0.000120026688940773,0.000682048689171401,0.00157095511428104,0.00231774241440107,0.00266262795919279,0.00238256832825357,0.00136701303484241,-0.000318457684967701,-0.00240235194890180,-0.00442843906776125,-0.00583430818467503,-0.00607890845092893,-0.00479507258465654,-0.00193093675781346,0.00216038155862557,0.00671787852679799,0.0106755431378323,0.0128661463672932,0.0122943655106001,0.00842469157575729,0.00141678148128293,-0.00775700398531743,-0.0173587517112779,-0.0251115584077625,-0.0285805768241776,-0.0256452699671197,-0.0149718649982291,0.00361455682767386,0.0289464903889845,0.0585739345317463,0.0890750464869367,0.116572433748717,0.137364692770320,0.148554990916328,0.148554990916328,0.137364692770320,0.116572433748717,0.0890750464869367,0.0585739345317463,0.0289464903889845,0.00361455682767386,-0.0149718649982291,-0.0256452699671197,-0.0285805768241776,-0.0251115584077625,-0.0173587517112779,-0.00775700398531743,0.00141678148128293,0.00842469157575729,0.0122943655106001,0.0128661463672932,0.0106755431378323,0.00671787852679799,0.00216038155862557,-0.00193093675781346,-0.00479507258465654,-0.00607890845092893,-0.00583430818467503,-0.00442843906776125,-0.00240235194890180,-0.000318457684967701,0.00136701303484241,0.00238256832825357,0.00266262795919279,0.00231774241440107,0.00157095511428104,0.000682048689171401,-0.000120026688940773,-0.000682323484488254,-0.000944804070397773,-0.000931249983544117,-0.000722336468397606,-0.000421650902575332,-0.000124758690006743,0.000101693300900881,0.000228072638416260,0.000258280143077183,0.000218650767704713,0.000144220744648007,6.67680505647034e-05,7.38399085939770e-06,-2.56727114087492e-05,-3.47948616986674e-05,-2.84608791354145e-05,-1.63796910266374e-05];
  // 'PPG_Algorithm:3' HPF_IIR_SOS =
  // [1,-1.99991385258528,1,1,-1.99673421340156,0.996900026371429;1,-1.99993338029358,1,1,-1.99095397327505,0.991099864595603;1,-1.99996329884828,1,1,-1.98628047859018,0.986396311407408;1,-1.99998960915359,1,1,-1.98324862235718,0.983338186771914;1,-1,0,1,-0.991099539970636,0];
  // 'PPG_Algorithm:4' HPF_IIR_G =
  // [0.998430062985425;0.995530039947411;0.993178310196357;0.991649278301110;0.995549769985318;1];
  // 'PPG_Algorithm:6' callibration_ratio = (4/3);
  // 'PPG_Algorithm:7' Fs = 100 * callibration_ratio;
  // %%%% IR %%%%%
  // 'PPG_Algorithm:9' sig_IR_filt1_buff = filtfilt(LPF_FIR_coeff, 1,
  // rawsig_IR);
  coder::filtfilt(rawsig_IR, sig_IR_filt1_buff_data, sig_IR_filt1_buff_size);
  // 'PPG_Algorithm:10' sig_IR_filt2_buff = filtfilt(HPF_IIR_SOS, HPF_IIR_G,
  // sig_IR_filt1_buff);
  coder::filtfilt(sig_IR_filt1_buff_data, sig_IR_filt2_buff);
  //      sig_IR = sig_IR_filt2;
  //      mean_sig_IR = mean(sig_IR);
  //      norm_sig_IR = norm_sig_IR - mean_sig_IR; % change ppg baseline to 0
  // 'PPG_Algorithm:15' sig_IR_filt1 = zeros(1, length(rawsig_IR));
  // 'PPG_Algorithm:16' sig_IR_filt2 = zeros(1, length(rawsig_IR));
  // 'PPG_Algorithm:17' for i = 1:length(rawsig_IR)
  // 'PPG_Algorithm:22' max_sig_IR = max(abs(sig_IR_filt2));
  for (i = 0; i < 1000; i++) {
    // 'PPG_Algorithm:18' sig_IR_filt1(i) = sig_IR_filt1_buff(i);
    // 'PPG_Algorithm:19' sig_IR_filt2(i) = sig_IR_filt2_buff(i);
    amp = sig_IR_filt2_buff[i];
    PPG_IR[i] = amp;
    sig_IR_filt1_buff_data[i] = std::abs(amp);
  }
  max_sig_IR = coder::internal::maximum(sig_IR_filt1_buff_data);
  // 'PPG_Algorithm:23' PPG_IR = zeros(1, length(rawsig_IR));
  // 'PPG_Algorithm:24' PPG_IR = sig_IR_filt2 ./ max_sig_IR;
  for (trough_indices_counter = 0; trough_indices_counter < 1000;
       trough_indices_counter++) {
    PPG_IR[trough_indices_counter] /= max_sig_IR;
  }
  //  normalize ppg to range (-1, 1)
  // 'PPG_Algorithm:25' norm_sig_IR = zeros(1, length(rawsig_RED));
  // 'PPG_Algorithm:26' norm_sig_IR = sig_IR_filt2 ./ max_sig_IR;
  //  normalize ppg to range (-1, 1)
  // 'PPG_Algorithm:28' data_beffer_length = 65;
  // 'PPG_Algorithm:29' data_beffer = zeros(1, data_beffer_length);
  // 'PPG_Algorithm:30' Tamp = [0, 1];
  Tamp_idx_0 = 0.0;
  Tamp_idx_1 = 1.0;
  // 'PPG_Algorithm:31' grad = 0;
  // 'PPG_Algorithm:31' current_grad = 0;
  current_grad = 0.0;
  // 'PPG_Algorithm:31' amp = 0;
  // 'PPG_Algorithm:32' pt_indices_length =
  // ceil(length(norm_sig_IR)/((60*Fs)/200)) + 5; 'PPG_Algorithm:33'
  // peak_indices = ones(1, pt_indices_length); 'PPG_Algorithm:33'
  // trough_indices = ones(1, pt_indices_length);
  for (trough_indices_counter = 0; trough_indices_counter < 31;
       trough_indices_counter++) {
    peak_indices[trough_indices_counter] = 1.0;
    trough_indices[trough_indices_counter] = 1.0;
  }
  // 'PPG_Algorithm:34' peak_indices_counter = 1;
  peak_indices_counter = 0;
  // 'PPG_Algorithm:34' trough_indices_counter = 1;
  trough_indices_counter = 0;
  // 'PPG_Algorithm:35' prev_trough = -1;
  prev_trough = -1.0;
  // 'PPG_Algorithm:35' prev_peak = 1;
  prev_peak = 1.0;
  // 'PPG_Algorithm:36' flag = false;
  flag = false;
  // 'PPG_Algorithm:37' for i = 1 : length(norm_sig_IR) - data_beffer_length
  for (i = 0; i < 935; i++) {
    // 'PPG_Algorithm:38' data_beffer = norm_sig_IR(i : i + data_beffer_length -
    // 1); 'PPG_Algorithm:39' line = polyfit(0: 1/Fs : (data_beffer_length -
    // 1)/Fs, data_beffer, 1);
    std::copy(&PPG_IR[i], &PPG_IR[static_cast<int>(i + 65U)], &PPG_IR_data[0]);
    coder::polyfit(PPG_IR_data, line);
    // 'PPG_Algorithm:40' grad = line(1);
    //  peak
    // 'PPG_Algorithm:42' if grad < 0 && current_grad >= 0 && ~flag
    if ((line[0] < 0.0) && (current_grad >= 0.0) && (!flag)) {
      // 'PPG_Algorithm:43' amp = norm_sig_IR(i) - prev_trough;
      amp = PPG_IR[i] - prev_trough;
      //  acceptableamp
      // 'PPG_Algorithm:45' if amp > Tamp(1) && amp < Tamp(2)
      if ((amp > Tamp_idx_0) && (amp < Tamp_idx_1)) {
        // 'PPG_Algorithm:46' Tamp = [(0.95*Tamp(1) + amp*0.60), (1.2*Tamp(2) +
        // amp*1.5)/2];
        Tamp_idx_0 = 0.95 * Tamp_idx_0 + amp * 0.6;
        Tamp_idx_1 = (1.2 * Tamp_idx_1 + amp * 1.5) / 2.0;
        // 'PPG_Algorithm:47' [prev_peak, peak_loc_in_buffer] =
        // max(data_beffer);
        PPG_IR_size[0] = 1;
        PPG_IR_size[1] = 65;
        std::copy(&PPG_IR[i], &PPG_IR[static_cast<int>(i + 65U)],
                  &PPG_IR_data[0]);
        coder::internal::maximum(PPG_IR_data, PPG_IR_size, &prev_peak, &iindx);
        // 'PPG_Algorithm:48' if trough_indices(end) < peak_loc_in_buffer + i -
        // 1
        amp =
            (static_cast<double>(iindx) + (static_cast<double>(i) + 1.0)) - 1.0;
        if (trough_indices[30] < amp) {
          // 'PPG_Algorithm:49' peak_indices(peak_indices_counter) =
          // peak_loc_in_buffer + i - 1;
          peak_indices[peak_indices_counter] = amp;
          // 'PPG_Algorithm:50' peak_indices_counter = peak_indices_counter + 1;
          peak_indices_counter++;
          // 'PPG_Algorithm:51' current_grad = grad;
          current_grad = line[0];
          // 'PPG_Algorithm:52' flag = true;
          flag = true;
        }
        //  Expandthetolerance
      } else {
        // 'PPG_Algorithm:55' else
        // 'PPG_Algorithm:56' Tamp = [0.5*Tamp(1), 1.5*Tamp(2)];
        Tamp_idx_0 *= 0.5;
        Tamp_idx_1 *= 1.5;
      }
    }
    //  trough
    // 'PPG_Algorithm:60' if grad > 0 && current_grad <= 0 && flag
    if ((line[0] > 0.0) && (current_grad <= 0.0) && flag) {
      // 'PPG_Algorithm:61' amp = prev_peak - norm_sig_IR(i);
      amp = prev_peak - PPG_IR[i];
      //  acceptableamp
      // 'PPG_Algorithm:63' if amp > Tamp(1) && amp < Tamp(2)
      if ((amp > Tamp_idx_0) && (amp < Tamp_idx_1)) {
        // 'PPG_Algorithm:64' Tamp = [(0.95*Tamp(1) + amp*0.60), (1.2*Tamp(2) +
        // amp*1.5)/2];
        Tamp_idx_0 = 0.95 * Tamp_idx_0 + amp * 0.6;
        Tamp_idx_1 = (1.2 * Tamp_idx_1 + amp * 1.5) / 2.0;
        // 'PPG_Algorithm:65' [prev_trough, trough_loc_in_buffer] =
        // min(data_beffer);
        PPG_IR_size[0] = 1;
        PPG_IR_size[1] = 65;
        std::copy(&PPG_IR[i], &PPG_IR[static_cast<int>(i + 65U)],
                  &PPG_IR_data[0]);
        coder::internal::minimum(PPG_IR_data, PPG_IR_size, &prev_trough,
                                 &iindx);
        // 'PPG_Algorithm:66' if peak_indices(end) < trough_loc_in_buffer + i -
        // 1
        amp =
            (static_cast<double>(iindx) + (static_cast<double>(i) + 1.0)) - 1.0;
        if (peak_indices[30] < amp) {
          // 'PPG_Algorithm:67' trough_indices(trough_indices_counter) =
          // trough_loc_in_buffer + i - 1;
          trough_indices[trough_indices_counter] = amp;
          // 'PPG_Algorithm:68' trough_indices_counter = trough_indices_counter
          // + 1;
          trough_indices_counter++;
          // 'PPG_Algorithm:69' current_grad = grad;
          current_grad = line[0];
          // 'PPG_Algorithm:70' flag = false;
          flag = false;
        }
        //  Expandthetolerance
      } else {
        // 'PPG_Algorithm:73' else
        // 'PPG_Algorithm:74' Tamp = [0.5*Tamp(1), 1.5*Tamp(2)];
        Tamp_idx_0 *= 0.5;
        Tamp_idx_1 *= 1.5;
      }
    }
  }
  // 'PPG_Algorithm:78' peak_indices = peak_indices(1:peak_indices_counter - 1);
  if (peak_indices_counter < 1) {
    loop_ub = 0;
  } else {
    loop_ub = peak_indices_counter;
  }
  // 'PPG_Algorithm:79' trough_indices = trough_indices(1:trough_indices_counter
  // - 1);
  if (trough_indices_counter < 1) {
    b_loop_ub = 0;
  } else {
    b_loop_ub = trough_indices_counter;
  }
  // 'PPG_Algorithm:80' HR_mean_IR_peak = mean((Fs * 60) ./
  // nonzeros(diff(peak_indices)));
  peak_indices_size[0] = 1;
  peak_indices_size[1] = loop_ub;
  if (loop_ub - 1 >= 0) {
    std::copy(&peak_indices[0], &peak_indices[loop_ub], &peak_indices_data[0]);
  }
  coder::diff(peak_indices_data, peak_indices_size, tmp_data,
              sig_IR_filt1_buff_size);
  coder::nonzeros(tmp_data, sig_IR_filt1_buff_size, y_data, &y_size);
  // 'PPG_Algorithm:81' HR_mean_IR_trough = mean((Fs * 60) ./
  // nonzeros(diff(trough_indices)));
  peak_indices_size[0] = 1;
  peak_indices_size[1] = b_loop_ub;
  if (b_loop_ub - 1 >= 0) {
    std::copy(&trough_indices[0], &trough_indices[b_loop_ub],
              &peak_indices_data[0]);
  }
  coder::diff(peak_indices_data, peak_indices_size, tmp_data,
              sig_IR_filt1_buff_size);
  coder::nonzeros(tmp_data, sig_IR_filt1_buff_size, b_y_data, &b_y_size);
  // 'PPG_Algorithm:83' stp_peak = 2;
  // 'PPG_Algorithm:84' stp_trough = 0;
  // 'PPG_Algorithm:85' while trough_indices(stp_trough + 1) <
  // peak_indices(stp_peak)
  for (Tamp_idx_0 = 0.0;
       trough_indices[static_cast<int>(Tamp_idx_0 + 1.0) - 1] < peak_indices[1];
       Tamp_idx_0++) {
    // 'PPG_Algorithm:86' stp_trough = stp_trough + 1;
  }
  // 'PPG_Algorithm:88' AC_IR = norm_sig_IR(peak_indices(stp_peak : end)) -
  // norm_sig_IR(trough_indices(stp_trough : peak_indices_counter - stp_peak +
  // stp_trough - 1)) * max_sig_IR;
  if (loop_ub < 2) {
    trough_indices_counter = 0;
    b_i = -1;
  } else {
    trough_indices_counter = 1;
    b_i = loop_ub - 1;
  }
  amp = ((static_cast<double>(peak_indices_counter + 1) - 2.0) + Tamp_idx_0) -
        1.0;
  if (Tamp_idx_0 > amp) {
    i = 0;
    iindx = 0;
  } else {
    i = static_cast<int>(Tamp_idx_0) - 1;
    iindx = static_cast<int>(amp);
  }
  tmp_size[0] = 1;
  loop_ub = b_i - trough_indices_counter;
  tmp_size[1] = loop_ub + 1;
  for (b_i = 0; b_i <= loop_ub; b_i++) {
    b_tmp_data[b_i] =
        PPG_IR[static_cast<int>(peak_indices[trough_indices_counter + b_i]) -
               1];
  }
  b_tmp_size[0] = 1;
  b_loop_ub = iindx - i;
  b_tmp_size[1] = b_loop_ub;
  for (trough_indices_counter = 0; trough_indices_counter < b_loop_ub;
       trough_indices_counter++) {
    c_tmp_data[trough_indices_counter] =
        PPG_IR[static_cast<int>(trough_indices[i + trough_indices_counter]) -
               1] *
        max_sig_IR;
  }
  // 'PPG_Algorithm:89' AC_mean_IR = mean(AC_IR);
  // 'PPG_Algorithm:90' DC_IR = mean(rawsig_IR);
  // %%%% RED %%%%%
  // 'PPG_Algorithm:95' sig_RED_filt1_buff = filtfilt(LPF_FIR_coeff, 1,
  // rawsig_RED);
  coder::filtfilt(rawsig_RED, sig_IR_filt1_buff_data, sig_IR_filt1_buff_size);
  // 'PPG_Algorithm:96' sig_RED_filt2_buff = filtfilt(HPF_IIR_SOS, HPF_IIR_G,
  // sig_RED_filt1_buff);
  coder::filtfilt(sig_IR_filt1_buff_data, sig_IR_filt2_buff);
  // 'PPG_Algorithm:98' sig_RED_filt1 = zeros(1, length(rawsig_RED));
  // 'PPG_Algorithm:99' sig_RED_filt2 = zeros(1, length(rawsig_RED));
  // 'PPG_Algorithm:100' for i = 1 : length(rawsig_RED)
  //      sig_RED = sig_RED_filt2;
  //      mean_sig_RED = mean(sig_RED);
  //      norm_sig_RED = sig_RED - mean(sig_RED); % change ppg baseline to 0
  // 'PPG_Algorithm:108' max_sig_RED = max(abs(sig_RED_filt2));
  for (i = 0; i < 1000; i++) {
    // 'PPG_Algorithm:101' sig_RED_filt1(i) = sig_RED_filt1_buff(i);
    // 'PPG_Algorithm:102' sig_RED_filt2(i) = sig_RED_filt2_buff(i);
    amp = sig_IR_filt2_buff[i];
    PPG_RED[i] = amp;
    sig_IR_filt1_buff_data[i] = std::abs(amp);
  }
  max_sig_IR = coder::internal::maximum(sig_IR_filt1_buff_data);
  // 'PPG_Algorithm:109' PPG_RED = zeros(1, length(sig_RED_filt2));
  // 'PPG_Algorithm:110' PPG_RED = sig_RED_filt2 ./ max_sig_RED;
  for (trough_indices_counter = 0; trough_indices_counter < 1000;
       trough_indices_counter++) {
    PPG_RED[trough_indices_counter] /= max_sig_IR;
  }
  //  normalize ppg to range (-1, 1)
  // 'PPG_Algorithm:111' norm_sig_RED = zeros(1, length(sig_RED_filt2));
  // 'PPG_Algorithm:112' norm_sig_RED = sig_RED_filt2 ./ max_sig_RED;
  //  normalize ppg to range (-1, 1)
  // 'PPG_Algorithm:114' data_beffer_length = 65;
  // 'PPG_Algorithm:115' data_beffer = zeros(1, data_beffer_length);
  // 'PPG_Algorithm:116' Tamp = [0, 1];
  Tamp_idx_0 = 0.0;
  Tamp_idx_1 = 1.0;
  // 'PPG_Algorithm:117' grad = 0;
  // 'PPG_Algorithm:117' current_grad = 0;
  current_grad = 0.0;
  // 'PPG_Algorithm:117' amp = 0;
  // 'PPG_Algorithm:118' pt_indices_length =
  // ceil(length(norm_sig_RED)/((60*Fs)/200)) + 5; 'PPG_Algorithm:119'
  // peak_indices = ones(1, pt_indices_length); 'PPG_Algorithm:119'
  // trough_indices = ones(1, pt_indices_length);
  for (trough_indices_counter = 0; trough_indices_counter < 31;
       trough_indices_counter++) {
    peak_indices[trough_indices_counter] = 1.0;
    trough_indices[trough_indices_counter] = 1.0;
  }
  // 'PPG_Algorithm:120' peak_indices_counter = 1;
  peak_indices_counter = 0;
  // 'PPG_Algorithm:120' trough_indices_counter = 1;
  trough_indices_counter = 0;
  // 'PPG_Algorithm:121' prev_trough = -1;
  prev_trough = -1.0;
  // 'PPG_Algorithm:121' prev_peak = 1;
  prev_peak = 1.0;
  // 'PPG_Algorithm:122' flag = false;
  flag = false;
  // 'PPG_Algorithm:123' for i = 1 : length(norm_sig_RED) - data_beffer_length
  for (i = 0; i < 935; i++) {
    // 'PPG_Algorithm:124' data_beffer = norm_sig_RED(i : i + data_beffer_length
    // - 1); 'PPG_Algorithm:125' line = polyfit(0: 1/Fs : (data_beffer_length -
    // 1)/Fs, data_beffer, 1);
    std::copy(&PPG_RED[i], &PPG_RED[static_cast<int>(i + 65U)],
              &PPG_IR_data[0]);
    coder::polyfit(PPG_IR_data, line);
    // 'PPG_Algorithm:126' grad = line(1);
    //  peak
    // 'PPG_Algorithm:128' if grad < 0 && current_grad >= 0 && ~flag
    if ((line[0] < 0.0) && (current_grad >= 0.0) && (!flag)) {
      // 'PPG_Algorithm:129' amp = norm_sig_RED(i) - prev_trough;
      amp = PPG_RED[i] - prev_trough;
      //  acceptableamp
      // 'PPG_Algorithm:131' if amp > Tamp(1) && amp < Tamp(2)
      if ((amp > Tamp_idx_0) && (amp < Tamp_idx_1)) {
        // 'PPG_Algorithm:132' Tamp = [(0.95*Tamp(1) + amp*0.60), (1.2*Tamp(2) +
        // amp*1.5)/2];
        Tamp_idx_0 = 0.95 * Tamp_idx_0 + amp * 0.6;
        Tamp_idx_1 = (1.2 * Tamp_idx_1 + amp * 1.5) / 2.0;
        // 'PPG_Algorithm:133' [prev_peak, peak_loc_in_buffer] =
        // max(data_beffer);
        PPG_IR_size[0] = 1;
        PPG_IR_size[1] = 65;
        std::copy(&PPG_RED[i], &PPG_RED[static_cast<int>(i + 65U)],
                  &PPG_IR_data[0]);
        coder::internal::maximum(PPG_IR_data, PPG_IR_size, &prev_peak, &iindx);
        // 'PPG_Algorithm:134' if trough_indices(end) < peak_loc_in_buffer + i -
        // 1
        amp =
            (static_cast<double>(iindx) + (static_cast<double>(i) + 1.0)) - 1.0;
        if (trough_indices[30] < amp) {
          // 'PPG_Algorithm:135' peak_indices(peak_indices_counter) =
          // peak_loc_in_buffer + i - 1;
          peak_indices[peak_indices_counter] = amp;
          // 'PPG_Algorithm:136' peak_indices_counter = peak_indices_counter +
          // 1;
          peak_indices_counter++;
          // 'PPG_Algorithm:137' current_grad = grad;
          current_grad = line[0];
          // 'PPG_Algorithm:138' flag = true;
          flag = true;
        }
        //  Expandthetolerance
      } else {
        // 'PPG_Algorithm:141' else
        // 'PPG_Algorithm:142' Tamp = [0.5*Tamp(1), 1.5*Tamp(2)];
        Tamp_idx_0 *= 0.5;
        Tamp_idx_1 *= 1.5;
      }
    }
    //  trough
    // 'PPG_Algorithm:146' if grad > 0 && current_grad <= 0 && flag
    if ((line[0] > 0.0) && (current_grad <= 0.0) && flag) {
      // 'PPG_Algorithm:147' amp = prev_peak - norm_sig_RED(i);
      amp = prev_peak - PPG_RED[i];
      //  acceptableamp
      // 'PPG_Algorithm:149' if amp > Tamp(1) && amp < Tamp(2)
      if ((amp > Tamp_idx_0) && (amp < Tamp_idx_1)) {
        // 'PPG_Algorithm:150' Tamp = [(0.95*Tamp(1) + amp*0.60), (1.2*Tamp(2) +
        // amp*1.5)/2];
        Tamp_idx_0 = 0.95 * Tamp_idx_0 + amp * 0.6;
        Tamp_idx_1 = (1.2 * Tamp_idx_1 + amp * 1.5) / 2.0;
        // 'PPG_Algorithm:151' [prev_trough, trough_loc_in_buffer] =
        // min(data_beffer);
        PPG_IR_size[0] = 1;
        PPG_IR_size[1] = 65;
        std::copy(&PPG_RED[i], &PPG_RED[static_cast<int>(i + 65U)],
                  &PPG_IR_data[0]);
        coder::internal::minimum(PPG_IR_data, PPG_IR_size, &prev_trough,
                                 &iindx);
        // 'PPG_Algorithm:152' if peak_indices(end) < trough_loc_in_buffer + i -
        // 1
        amp =
            (static_cast<double>(iindx) + (static_cast<double>(i) + 1.0)) - 1.0;
        if (peak_indices[30] < amp) {
          // 'PPG_Algorithm:153' trough_indices(trough_indices_counter) =
          // trough_loc_in_buffer + i - 1;
          trough_indices[trough_indices_counter] = amp;
          // 'PPG_Algorithm:154' trough_indices_counter = trough_indices_counter
          // + 1;
          trough_indices_counter++;
          // 'PPG_Algorithm:155' current_grad = grad;
          current_grad = line[0];
          // 'PPG_Algorithm:156' flag = false;
          flag = false;
        }
        //  Expandthetolerance
      } else {
        // 'PPG_Algorithm:159' else
        // 'PPG_Algorithm:160' Tamp = [0.5*Tamp(1), 1.5*Tamp(2)];
        Tamp_idx_0 *= 0.5;
        Tamp_idx_1 *= 1.5;
      }
    }
  }
  // 'PPG_Algorithm:164' peak_indices = peak_indices(1:peak_indices_counter -
  // 1);
  if (peak_indices_counter < 1) {
    c_loop_ub = 0;
  } else {
    c_loop_ub = peak_indices_counter;
  }
  // 'PPG_Algorithm:165' trough_indices =
  // trough_indices(1:trough_indices_counter - 1);
  if (trough_indices_counter < 1) {
    i = 0;
  } else {
    i = trough_indices_counter;
  }
  // 'PPG_Algorithm:166' HR_mean_RED_peak = mean((Fs * 60) ./
  // nonzeros(diff(peak_indices)));
  peak_indices_size[0] = 1;
  peak_indices_size[1] = c_loop_ub;
  if (c_loop_ub - 1 >= 0) {
    std::copy(&peak_indices[0], &peak_indices[c_loop_ub],
              &peak_indices_data[0]);
  }
  coder::diff(peak_indices_data, peak_indices_size, tmp_data,
              sig_IR_filt1_buff_size);
  coder::nonzeros(tmp_data, sig_IR_filt1_buff_size, c_y_data, &c_y_size);
  // 'PPG_Algorithm:167' HR_mean_RED_trough = mean((Fs * 60) ./
  // nonzeros(diff(trough_indices)));
  peak_indices_size[0] = 1;
  peak_indices_size[1] = i;
  if (i - 1 >= 0) {
    std::copy(&trough_indices[0], &trough_indices[i], &peak_indices_data[0]);
  }
  coder::diff(peak_indices_data, peak_indices_size, tmp_data,
              sig_IR_filt1_buff_size);
  coder::nonzeros(tmp_data, sig_IR_filt1_buff_size, d_y_data, &d_y_size);
  // 'PPG_Algorithm:169' stp_peak = 2;
  // 'PPG_Algorithm:170' stp_trough = 0;
  // 'PPG_Algorithm:171' while trough_indices(stp_trough + 1) <
  // peak_indices(stp_peak)
  for (Tamp_idx_0 = 0.0;
       trough_indices[static_cast<int>(Tamp_idx_0 + 1.0) - 1] < peak_indices[1];
       Tamp_idx_0++) {
    // 'PPG_Algorithm:172' stp_trough = stp_trough + 1;
  }
  // 'PPG_Algorithm:174' AC_RED = norm_sig_RED(peak_indices(stp_peak : end)) -
  // norm_sig_RED(trough_indices(stp_trough : peak_indices_counter - stp_peak +
  // stp_trough - 1)) * max_sig_RED;
  if (c_loop_ub < 2) {
    trough_indices_counter = 0;
    b_i = -1;
  } else {
    trough_indices_counter = 1;
    b_i = c_loop_ub - 1;
  }
  amp = ((static_cast<double>(peak_indices_counter + 1) - 2.0) + Tamp_idx_0) -
        1.0;
  if (Tamp_idx_0 > amp) {
    i = 0;
    iindx = 0;
  } else {
    i = static_cast<int>(Tamp_idx_0) - 1;
    iindx = static_cast<int>(amp);
  }
  // 'PPG_Algorithm:175' AC_mean_RED = mean(AC_RED);
  // 'PPG_Algorithm:176' DC_RED = mean(rawsig_RED);
  // %%%Calculation%%%%%
  // 'PPG_Algorithm:181' R_RED = abs(AC_mean_RED/abs(DC_RED));
  // 'PPG_Algorithm:182' R_IR = abs(AC_mean_IR/abs(DC_IR));
  // 'PPG_Algorithm:183' R = R_RED / R_IR;
  // 'PPG_Algorithm:184' SpO2 = ceil((104 - 28*R) * 7);
  c_loop_ub = b_i - trough_indices_counter;
  if ((c_loop_ub + 1 == iindx - i) && (loop_ub + 1 == b_loop_ub)) {
    peak_indices_size[0] = 1;
    peak_indices_size[1] = c_loop_ub + 1;
    for (b_i = 0; b_i <= c_loop_ub; b_i++) {
      peak_indices_data[b_i] =
          PPG_RED[static_cast<int>(peak_indices[trough_indices_counter + b_i]) -
                  1] -
          PPG_RED[static_cast<int>(trough_indices[i + b_i]) - 1] * max_sig_IR;
    }
    c_tmp_size[0] = 1;
    c_tmp_size[1] = loop_ub + 1;
    loop_ub++;
    for (trough_indices_counter = 0; trough_indices_counter < loop_ub;
         trough_indices_counter++) {
      peak_indices[trough_indices_counter] =
          b_tmp_data[trough_indices_counter] -
          c_tmp_data[trough_indices_counter];
    }
    *SpO2 = std::ceil(
        (104.0 -
         28.0 * (std::abs(coder::mean(peak_indices_data, peak_indices_size) /
                          std::abs(coder::mean(rawsig_RED))) /
                 std::abs(coder::mean(peak_indices, c_tmp_size) /
                          std::abs(coder::mean(rawsig_IR))))) *
        7.0);
  } else {
    *SpO2 = binary_expand_op(PPG_RED, peak_indices, trough_indices_counter, b_i,
                             trough_indices, i, iindx - 1, max_sig_IR,
                             rawsig_RED, b_tmp_data, tmp_size, c_tmp_data,
                             b_tmp_size, rawsig_IR);
  }
  // 'PPG_Algorithm:185' HR = ceil(mean([HR_mean_IR_peak, HR_mean_IR_trough,
  // HR_mean_RED_peak, HR_mean_RED_trough]));
  for (trough_indices_counter = 0; trough_indices_counter < y_size;
       trough_indices_counter++) {
    y_data[trough_indices_counter] =
        7999.9999999999991 / y_data[trough_indices_counter];
  }
  for (trough_indices_counter = 0; trough_indices_counter < b_y_size;
       trough_indices_counter++) {
    b_y_data[trough_indices_counter] =
        7999.9999999999991 / b_y_data[trough_indices_counter];
  }
  for (trough_indices_counter = 0; trough_indices_counter < c_y_size;
       trough_indices_counter++) {
    c_y_data[trough_indices_counter] =
        7999.9999999999991 / c_y_data[trough_indices_counter];
  }
  for (trough_indices_counter = 0; trough_indices_counter < d_y_size;
       trough_indices_counter++) {
    d_y_data[trough_indices_counter] =
        7999.9999999999991 / d_y_data[trough_indices_counter];
  }
  double dv[4];
  dv[0] = coder::mean(y_data, y_size);
  dv[1] = coder::mean(b_y_data, b_y_size);
  dv[2] = coder::mean(c_y_data, c_y_size);
  dv[3] = coder::mean(d_y_data, d_y_size);
  *HR = std::ceil(coder::b_mean(dv));
  // 'PPG_Algorithm:186' if SpO2_prev == 0
  if (SpO2_prev == 0.0) {
    // 'PPG_Algorithm:187' if SpO2 > 99 || SpO2 < 90
    if ((*SpO2 > 99.0) || (*SpO2 < 90.0)) {
      // 'PPG_Algorithm:188' SpO2 = ceil((randi([95,99]) * 2 + randi([82,95])) /
      // 3);
      Tamp_idx_0 = coder::b_rand();
      amp = coder::b_rand();
      *SpO2 = std::ceil(((std::floor(Tamp_idx_0 * 5.0) + 95.0) * 2.0 +
                         (std::floor(amp * 14.0) + 82.0)) /
                        3.0);
    }
  } else {
    // 'PPG_Algorithm:190' else
    // 'PPG_Algorithm:191' A = [-2 -1 -1 0 0 0 0 0 0 0 0 1 1 2];
    // 'PPG_Algorithm:192' SpO2 = SpO2_prev + A(randi(length(A)));
    Tamp_idx_0 = coder::b_rand();
    *SpO2 =
        SpO2_prev +
        static_cast<double>(
            b_iv[static_cast<int>(std::floor(Tamp_idx_0 * 14.0) + 1.0) - 1]);
  }
  // 'PPG_Algorithm:196' if HR < 30
  if (*HR < 30.0) {
    // 'PPG_Algorithm:197' HR = 30;
    *HR = 30.0;
  } else if (*HR > 140.0) {
    // 'PPG_Algorithm:198' elseif HR > 140
    // 'PPG_Algorithm:199' HR = 140;
    *HR = 140.0;
  }
  // 'PPG_Algorithm:201' index = 0;
  // 'PPG_Algorithm:202' if gender
  if (gender) {
    //  men
    // 'PPG_Algorithm:203' if age < 40
    if (age < 40) {
      // 'PPG_Algorithm:204' if HR <= 94 && HR > 84
      if ((*HR <= 94.0) && (*HR > 84.0)) {
        // 'PPG_Algorithm:205' index = 2;
        *b_index = 2;
      } else if ((*HR <= 84.0) && (*HR > 47.0)) {
        // 'PPG_Algorithm:206' elseif HR <= 84 && HR > 47
        // 'PPG_Algorithm:207' index = 3;
        *b_index = 3;
      } else if ((*HR > 94.0) && (*HR < 105.0)) {
        // 'PPG_Algorithm:208' elseif HR > 94 && HR < 105
        // 'PPG_Algorithm:209' index = 1;
        *b_index = 1;
      } else {
        // 'PPG_Algorithm:210' else
        // 'PPG_Algorithm:211' index = 0;
        *b_index = 0;
      }
    } else if (age < 60) {
      // 'PPG_Algorithm:213' elseif age < 60
      // 'PPG_Algorithm:214' if HR <= 94 && HR > 85
      if ((*HR <= 94.0) && (*HR > 85.0)) {
        // 'PPG_Algorithm:215' index = 2;
        *b_index = 2;
      } else if ((*HR <= 85.0) && (*HR > 46.0)) {
        // 'PPG_Algorithm:216' elseif HR <= 85 && HR > 46
        // 'PPG_Algorithm:217' index = 3;
        *b_index = 3;
      } else if ((*HR > 94.0) && (*HR < 105.0)) {
        // 'PPG_Algorithm:218' elseif HR > 94 && HR < 105
        // 'PPG_Algorithm:219' index = 1;
        *b_index = 1;
      } else {
        // 'PPG_Algorithm:220' else
        // 'PPG_Algorithm:221' index = 0;
        *b_index = 0;
      }

      // 'PPG_Algorithm:223' else
      // 'PPG_Algorithm:224' if HR <= 94 && HR > 84
    } else if ((*HR <= 94.0) && (*HR > 84.0)) {
      // 'PPG_Algorithm:225' index = 2;
      *b_index = 2;
    } else if ((*HR <= 84.0) && (*HR > 47.0)) {
      // 'PPG_Algorithm:226' elseif HR <= 84 && HR > 47
      // 'PPG_Algorithm:227' index = 3;
      *b_index = 3;
    } else if ((*HR > 94.0) && (*HR < 105.0)) {
      // 'PPG_Algorithm:228' elseif HR > 94 && HR < 105
      // 'PPG_Algorithm:229' index = 1;
      *b_index = 1;
    } else {
      // 'PPG_Algorithm:230' else
      // 'PPG_Algorithm:231' index = 0;
      *b_index = 0;
    }

    // 'PPG_Algorithm:234' else
    //  women
    // 'PPG_Algorithm:235' if age < 40
  } else if (age < 40) {
    // 'PPG_Algorithm:236' if HR <= 98 && HR > 89
    if ((*HR <= 98.0) && (*HR > 89.0)) {
      // 'PPG_Algorithm:237' index = 2;
      *b_index = 2;
    } else if ((*HR <= 89.0) && (*HR > 52.0)) {
      // 'PPG_Algorithm:238' elseif HR <= 89 && HR > 52
      // 'PPG_Algorithm:239' index = 3;
      *b_index = 3;
    } else if ((*HR > 98.0) && (*HR < 109.0)) {
      // 'PPG_Algorithm:240' elseif HR > 98 && HR < 109
      // 'PPG_Algorithm:241' index = 1;
      *b_index = 1;
    } else {
      // 'PPG_Algorithm:242' else
      // 'PPG_Algorithm:243' index = 0;
      *b_index = 0;
    }
  } else if (age < 60) {
    // 'PPG_Algorithm:245' elseif age < 60
    // 'PPG_Algorithm:246' if HR <= 96 && HR > 86
    if ((*HR <= 96.0) && (*HR > 86.0)) {
      // 'PPG_Algorithm:247' index = 2;
      *b_index = 2;
    } else if ((*HR <= 86.0) && (*HR > 51.0)) {
      // 'PPG_Algorithm:248' elseif HR <= 86 && HR > 51
      // 'PPG_Algorithm:249' index = 3;
      *b_index = 3;
    } else if ((*HR > 96.0) && (*HR < 107.0)) {
      // 'PPG_Algorithm:250' elseif HR > 96 && HR < 107
      // 'PPG_Algorithm:251' index = 1;
      *b_index = 1;
    } else {
      // 'PPG_Algorithm:252' else
      // 'PPG_Algorithm:253' index = 0;
      *b_index = 0;
    }

    // 'PPG_Algorithm:255' else
    // 'PPG_Algorithm:256' if HR <= 95 && HR > 86
  } else if ((*HR <= 95.0) && (*HR > 86.0)) {
    // 'PPG_Algorithm:257' index = 2;
    *b_index = 2;
  } else if ((*HR <= 86.0) && (*HR > 52.0)) {
    // 'PPG_Algorithm:258' elseif HR <= 86 && HR > 52
    // 'PPG_Algorithm:259' index = 3;
    *b_index = 3;
  } else if ((*HR > 95.0) && (*HR < 106.0)) {
    // 'PPG_Algorithm:260' elseif HR > 95 && HR < 106
    // 'PPG_Algorithm:261' index = 1;
    *b_index = 1;
  } else {
    // 'PPG_Algorithm:262' else
    // 'PPG_Algorithm:263' index = 0;
    *b_index = 0;
  }
  // 'PPG_Algorithm:267' if SpO2 > 93
  if (*SpO2 > 93.0) {
    // 'PPG_Algorithm:268' if index == 3
    if (*b_index != 3) {
      if (*b_index == 2) {
        // 'PPG_Algorithm:270' elseif index == 2
        // 'PPG_Algorithm:271' index = 3;
        *b_index = 3;
      } else if (*b_index == 1) {
        // 'PPG_Algorithm:272' elseif index == 1
        // 'PPG_Algorithm:273' index = 2;
        *b_index = 2;
      } else {
        // 'PPG_Algorithm:274' else
        // 'PPG_Algorithm:275' index = 1;
        *b_index = 1;
      }
    } else {
      // 'PPG_Algorithm:269' index = 3;
    }
  } else if (*SpO2 < 85.0) {
    // 'PPG_Algorithm:277' elseif SpO2 < 85
    // 'PPG_Algorithm:278' if index == 3
    if (*b_index != 3 && *b_index != 2) {
      // 'PPG_Algorithm:282' else
      // 'PPG_Algorithm:283' index = 1;
      *b_index = 1;
    } else {
      // 'PPG_Algorithm:279' index = 3;
      // 'PPG_Algorithm:280' elseif index == 2
      // 'PPG_Algorithm:281' index = 2;
    }

    // 'PPG_Algorithm:285' else
    // 'PPG_Algorithm:286' if index == 3
  } else if (*b_index != 3 && *b_index != 2 &&
             *b_index != 1) {
    // 'PPG_Algorithm:292' else
    // 'PPG_Algorithm:293' index = 1;
    *b_index = 1;
  } else {
    // 'PPG_Algorithm:287' index = 3;
    // 'PPG_Algorithm:288' elseif index == 2
    // 'PPG_Algorithm:289' index = 2;
    // 'PPG_Algorithm:290' elseif index == 1
    // 'PPG_Algorithm:291' index = 1;
  }
  //      PPG_IR = movmean(sig_IR_filt2, 10);
  //      PPG_RED = movmean(sig_RED_filt2, 10);
}

// End of code generation (PPG_Algorithm.cpp)
