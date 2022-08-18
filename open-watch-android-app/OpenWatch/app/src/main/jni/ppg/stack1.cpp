//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// stack1.cpp
//
// Code generation for function 'stack1'
//

// Include files
#include "stack1.h"
#include "PPG_Algorithm_internal_types.h"

// Function Definitions
namespace coder {
namespace internal {
void stack::init()
{
  static const struct_T r{
      1,   // xstart
      301, // xend
      0    // depth
  };
  for (int i{0}; i < 32; i++) {
    d[i] = r;
  }
  n = 0;
}

} // namespace internal
} // namespace coder

// End of code generation (stack1.cpp)
