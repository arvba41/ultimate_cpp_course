//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: pmsm_initialize.cpp
//
// MATLAB Coder version            : 23.2
// C/C++ source code generated on  : 16-Oct-2024 14:44:29
//

// Include Files
#include "pmsm_initialize.h"
#include "CoderTimeAPI.h"
#include "pmsm_data.h"

// Function Definitions
//
// Arguments    : void
// Return Type  : void
//
void pmsm_initialize()
{
  CoderTimeAPI::callCoderClockGettime_init();
  isInitialized_pmsm = true;
}

//
// File trailer for pmsm_initialize.cpp
//
// [EOF]
//
