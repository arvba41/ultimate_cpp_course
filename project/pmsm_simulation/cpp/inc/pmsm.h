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
  int np;       // pole pairs
  double J;     // inertia

  // vectors
  VectorXd t; // time vector
  MatrixXd x; // state vector (3xN), x[0] -> id, x[1] -> iq, x[2] -> omega
  MatrixXd u; // input vector (3xN), u[0] -> vd, u[1] -> vq, u[2] -> Tl
  // MatrixXd xdot = MatrixXd::Zero(3, 1); // state derivatives
  Matrix<double, 3, 1> xdot; // state derivatives

  // constructor
  PMSM(const double &Ld, const double &Lq, const double &Rs,
       const double &psi_r, const int &np, const double &J) {
    this->Ld = Ld;
    this->Lq = Lq;
    this->Rs = Rs;
    this->psi_r = psi_r;
    this->np = np;
    this->J = J;
  }

  // public methods
  MatrixXd fx(const Matrix<double, 3, 1> &x,
              const Matrix<double, 3, 1> &u); // State equations

  void set_time(const double &start, const double &end,
                const double &delta); // set time vector

  void simulate(const Vector3d &x0); // simulate the PMSM

  void set_inputs(const int &input_idx, const double &val,
                  const double &t0); // set input vector

private:
  // variables
  // double t_now; // current time

  // private methods
  VectorXd UnitStep(const double &t0); // unit step function
};

#endif // PMSM_H