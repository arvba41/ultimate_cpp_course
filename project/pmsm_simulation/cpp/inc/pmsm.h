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
  MatrixXd xdot = MatrixXd::Zero(3, 1); // state derivatives

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

#endif // PMSM_H