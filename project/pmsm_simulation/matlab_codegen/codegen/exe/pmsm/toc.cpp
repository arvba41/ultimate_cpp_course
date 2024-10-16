//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: toc.cpp
//
// MATLAB Coder version            : 23.2
// C/C++ source code generated on  : 16-Oct-2024 14:44:29
//

// Include Files
#include "toc.h"
#include "pmsm_data.h"
#include "coder_posix_time.h"

// Function Definitions
//
// Arguments    : double tstart_tv_sec
//                double tstart_tv_nsec
// Return Type  : double
//
namespace coder {
double toc(double tstart_tv_sec, double tstart_tv_nsec)
{
  coderTimespec b_timespec;
  if (!freq_not_empty) {
    freq_not_empty = true;
    coderInitTimeFunctions(&freq);
  }
  coderTimeClockGettimeMonotonic(&b_timespec, freq);
  return (b_timespec.tv_sec - tstart_tv_sec) +
         (b_timespec.tv_nsec - tstart_tv_nsec) / 1.0E+9;
}

} // namespace coder

//
// File trailer for toc.cpp
//
// [EOF]
//
