//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// sparse.h
//
// Code generation for function 'sparse'
//

#ifndef SPARSE_H
#define SPARSE_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Definitions
namespace coder {
class sparse {
public:
  array<double, 1U> d;
  array<int, 1U> colidx;
  array<int, 1U> rowidx;
};

} // namespace coder

#endif
// End of code generation (sparse.h)
