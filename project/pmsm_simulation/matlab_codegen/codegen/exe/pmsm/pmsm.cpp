//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: pmsm.cpp
//
// MATLAB Coder version            : 23.2
// C/C++ source code generated on  : 16-Oct-2024 14:44:29
//

// Include Files
#include "pmsm.h"
#include "pmsm_data.h"
#include "pmsm_initialize.h"
#include "pmsm_types.h"
#include "tic.h"
#include "toc.h"
#include "coder_array.h"
#include <cstdio>
#include <cstring>

// Function Declarations
static void set_input(const double t[100001], double u[300003]);

// Function Definitions
//
// Arguments    : const double t[100001]
//                double u[300003]
// Return Type  : void
//
static void set_input(const double t[100001], double u[300003])
{
  coder::array<int, 2U> r;
  int trueCount;
  std::memset(&u[0], 0, 300003U * sizeof(double));
  trueCount = 0;
  for (int i{0}; i < 100001; i++) {
    if (t[i] >= 0.1) {
      trueCount++;
    }
  }
  r.set_size(1, trueCount);
  trueCount = 0;
  for (int i{0}; i < 100001; i++) {
    if (t[i] >= 0.1) {
      r[trueCount] = i;
      trueCount++;
    }
  }
  trueCount = r.size(1);
  for (int i{0}; i < trueCount; i++) {
    u[3 * r[i] + 1] = 100.0;
  }
}

//
// motor parameters
//
// Arguments    : struct0_T *out
// Return Type  : void
//
void pmsm(struct0_T *out)
{
  double kd;
  double t1_tv_nsec;
  double t1_tv_sec;
  if (!isInitialized_pmsm) {
    pmsm_initialize();
  }
  //  d-axis inductance
  //  q-axis inductance
  //  stator resistance
  //  rotor flux linkage
  //  number of pole pairs
  //  rotor inertia
  //  Functions
  out->t[0] = 0.0;
  out->t[100000] = 1.0;
  for (int k{0}; k < 49999; k++) {
    kd = (static_cast<double>(k) + 1.0) * 1.0E-5;
    out->t[k + 1] = kd;
    out->t[99999 - k] = 1.0 - kd;
  }
  out->t[50000] = 0.5;
  set_input(out->t, out->u);
  std::memset(&out->x[0], 0, 300003U * sizeof(double));
  out->x[0] = 0.0;
  out->x[1] = 0.0;
  out->x[2] = 0.0;
  t1_tv_sec = coder::tic(t1_tv_nsec);
  for (int k{0}; k < 100000; k++) {
    double b_k1_idx_0_tmp;
    double b_k1_idx_0_tmp_tmp;
    double c_k1_idx_0_tmp;
    double d_k1_idx_0_tmp;
    double h;
    double k1_idx_0;
    double k1_idx_1_tmp;
    double k1_idx_2;
    double k1_idx_2_tmp;
    double k2_idx_0;
    double k2_idx_1;
    double k2_idx_2;
    double k3_idx_0;
    double k3_idx_1;
    double k3_idx_2;
    double x_idx_0;
    double x_idx_1;
    double x_idx_2;
    int i;
    int k1_idx_0_tmp;
    int k1_idx_0_tmp_tmp;
    //  Runge-Kutta 4th order solver
    h = out->t[k + 1] - out->t[k];
    //  state-space model of PMSM
    //  x = [id; iq; omega]
    //  u = [v_d; v_q]
    //  d-axis voltage equation
    k1_idx_0_tmp = 3 * k + 1;
    b_k1_idx_0_tmp = out->x[k1_idx_0_tmp];
    k1_idx_0_tmp_tmp = 3 * k + 2;
    b_k1_idx_0_tmp_tmp = out->x[k1_idx_0_tmp_tmp];
    kd = 3.0 * b_k1_idx_0_tmp_tmp;
    c_k1_idx_0_tmp = out->x[3 * k];
    d_k1_idx_0_tmp = out->u[3 * k];
    k1_idx_0 =
        h * (350.87719298245611 * ((d_k1_idx_0_tmp - 0.054 * c_k1_idx_0_tmp) +
                                   kd * 0.00285 * b_k1_idx_0_tmp));
    k1_idx_1_tmp = out->u[k1_idx_0_tmp];
    kd = h * (350.87719298245611 * ((k1_idx_1_tmp - 0.054 * b_k1_idx_0_tmp) -
                                    kd * (0.00285 * c_k1_idx_0_tmp + 0.8603)));
    k1_idx_2_tmp = out->u[k1_idx_0_tmp_tmp];
    k1_idx_2 = h * (4.0 * (4.5 * (0.8603 * b_k1_idx_0_tmp +
                                  0.0 * c_k1_idx_0_tmp * b_k1_idx_0_tmp) -
                           k1_idx_2_tmp));
    x_idx_0 = c_k1_idx_0_tmp + 0.5 * k1_idx_0;
    x_idx_1 = b_k1_idx_0_tmp + 0.5 * kd;
    x_idx_2 = b_k1_idx_0_tmp_tmp + 0.5 * k1_idx_2;
    //  state-space model of PMSM
    //  x = [id; iq; omega]
    //  u = [v_d; v_q]
    //  d-axis voltage equation
    k2_idx_0 = h * (350.87719298245611 * ((d_k1_idx_0_tmp - 0.054 * x_idx_0) +
                                          3.0 * x_idx_2 * 0.00285 * x_idx_1));
    k2_idx_1 = h * (350.87719298245611 *
                    ((k1_idx_1_tmp - 0.054 * x_idx_1) -
                     3.0 * x_idx_2 * (0.00285 * x_idx_0 + 0.8603)));
    k2_idx_2 = h * (4.0 * (4.5 * (0.8603 * x_idx_1 + 0.0 * x_idx_0 * x_idx_1) -
                           k1_idx_2_tmp));
    x_idx_0 = c_k1_idx_0_tmp + 0.5 * k2_idx_0;
    x_idx_1 = b_k1_idx_0_tmp + 0.5 * k2_idx_1;
    x_idx_2 = b_k1_idx_0_tmp_tmp + 0.5 * k2_idx_2;
    //  state-space model of PMSM
    //  x = [id; iq; omega]
    //  u = [v_d; v_q]
    //  d-axis voltage equation
    k3_idx_0 = h * (350.87719298245611 * ((d_k1_idx_0_tmp - 0.054 * x_idx_0) +
                                          3.0 * x_idx_2 * 0.00285 * x_idx_1));
    k3_idx_1 = h * (350.87719298245611 *
                    ((k1_idx_1_tmp - 0.054 * x_idx_1) -
                     3.0 * x_idx_2 * (0.00285 * x_idx_0 + 0.8603)));
    k3_idx_2 = h * (4.0 * (4.5 * (0.8603 * x_idx_1 + 0.0 * x_idx_0 * x_idx_1) -
                           k1_idx_2_tmp));
    x_idx_0 = c_k1_idx_0_tmp + k3_idx_0;
    x_idx_1 = b_k1_idx_0_tmp + k3_idx_1;
    x_idx_2 = b_k1_idx_0_tmp_tmp + k3_idx_2;
    //  state-space model of PMSM
    //  x = [id; iq; omega]
    //  u = [v_d; v_q]
    //  d-axis voltage equation
    i = 3 * (k + 1);
    out->x[i] =
        c_k1_idx_0_tmp +
        (((k1_idx_0 + 2.0 * k2_idx_0) + 2.0 * k3_idx_0) +
         h * (350.87719298245611 * ((d_k1_idx_0_tmp - 0.054 * x_idx_0) +
                                    3.0 * x_idx_2 * 0.00285 * x_idx_1))) /
            6.0;
    out->x[i + 1] = out->x[k1_idx_0_tmp] +
                    (((kd + 2.0 * k2_idx_1) + 2.0 * k3_idx_1) +
                     h * (350.87719298245611 *
                          ((k1_idx_1_tmp - 0.054 * x_idx_1) -
                           3.0 * x_idx_2 * (0.00285 * x_idx_0 + 0.8603)))) /
                        6.0;
    out->x[i + 2] =
        out->x[k1_idx_0_tmp_tmp] +
        (((k1_idx_2 + 2.0 * k2_idx_2) + 2.0 * k3_idx_2) +
         h * (4.0 * (4.5 * (0.8603 * x_idx_1 + 0.0 * x_idx_0 * x_idx_1) -
                     k1_idx_2_tmp))) /
            6.0;
  }
  kd = coder::toc(t1_tv_sec, t1_tv_nsec);
  std::printf("Elapsed time is %.2f ms\n", kd * 1000.0);
  std::fflush(stdout);
}

//
// File trailer for pmsm.cpp
//
// [EOF]
//
