#include "pmsm.h"

#include <eigen3/Eigen/Dense>

using namespace Eigen;

/* Electric macnine function definitions */
MatrixXd PMSM::fx(const MatrixXd &x, const MatrixXd &u) { // State equations
  // extract states
  double id = x(0);
  double iq = x(1);
  double omega = x(2);

  // extract inputs
  double vd = u(0);
  double vq = u(1);
  double Tl = u(2);

  // state equations
  double id_dot =
      1 / Ld * (vd - Rs * id + np * omega * Lq * iq); // d-axis current update
  double iq_dot =
      1 / Lq *
      (vq - Rs * iq - np * omega * (Ld * id + psi_r)); // q-axis current update
  double omega_dot = 1 / J *
                     (3 * np / 2 * (psi_r * iq + (Ld - Lq) * id * iq) -
                      Tl); // mechanical speed update

  xdot << id_dot, iq_dot, omega_dot;

  return xdot;
}

void PMSM::set_time(const double &start, const double &end,
                    const double &delta) { // set time vector
  t = VectorXd::LinSpaced((end - start) / delta, start, end);
}

void PMSM::simulate(const VectorXd &x0) { // simulate the PMSM<
  x = MatrixXd::Zero(3, t.size());
  x.col(0) = x0;

  RungeKutta4<PMSM>(t, x, u, *this);
}

void PMSM::set_inputs(const int &input_idx, const double &val,
                      const double &t0) { // set input vector
  u = MatrixXd::Zero(3, t.size());
  u.row(input_idx) = UnitStep(t0) * val;
}

VectorXd PMSM::UnitStep(const double &t0) { // unit step function
  VectorXd u = VectorXd::Zero(t.size());    // initialize with zeros
  for (int ii = 0; ii < t.size(); ++ii) {
    if (t[ii] > t0) {
      u[ii] = 1;
    }
  }
  return u;
}