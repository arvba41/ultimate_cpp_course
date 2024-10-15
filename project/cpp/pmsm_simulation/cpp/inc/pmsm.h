#ifndef PMSM_H
#define PMSM_H

#include <eigen3/Eigen/Dense>

#include "simulators.h"

using namespace Eigen;

/* PMSM class */
class PMSM {
public:
  double Ld;    // d-axis inductance
  double Lq;    // q-axis inductance
  double Rs;    // stator resistance
  double psi_r; // rotor flux
  double np;    // pole pairs
  double J;     // inertia

  // vectors
  VectorXd t; // time vector
  MatrixXd x; // state vector (3xN), x[0] -> id, x[1] -> iq, x[2] -> omega
  MatrixXd u; // input vector (3xN), u[0] -> vd, u[1] -> vq, u[2] -> Tl

  // public methods
  MatrixXd fx(const MatrixXd &x, const MatrixXd &u); // State equations

  void set_time(const double &start, const double &end,
                const double &delta); // set time vector

  void simulate(const VectorXd &x0); // simulate the PMSM

  void set_inputs(const int &input_idx, const double &val,
                  const double &t0); // set input vector

private:
  // private methods
  VectorXd UnitStep(const double &t0); // unit step function
};

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

  // return the state derivatives
  MatrixXd xdot(3, 1);
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

  RungeKutta4(t, x, u, *this);
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

#endif // PMSM_H