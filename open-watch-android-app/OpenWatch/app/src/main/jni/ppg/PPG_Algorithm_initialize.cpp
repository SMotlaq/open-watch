//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// PPG_Algorithm_initialize.cpp
//
// Code generation for function 'PPG_Algorithm_initialize'
//

// Include files
#include "PPG_Algorithm_initialize.h"
#include "PPG_Algorithm_data.h"
#include "eml_rand_mt19937ar_stateful.h"

// Function Definitions
void PPG_Algorithm_initialize()
{
  eml_rand_mt19937ar_stateful_init();
  isInitialized_PPG_Algorithm = true;
}

// End of code generation (PPG_Algorithm_initialize.cpp)
