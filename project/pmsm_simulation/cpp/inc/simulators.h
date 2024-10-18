#ifndef SIMULATORS_H
#define SIMULATORS_H

#include <eigen3/Eigen/Dense>

using namespace Eigen;

// Euler forward method
template <typename ODE> // ODE function class
void euler_forward(const VectorXd &t, MatrixXd &x, const MatrixXd &u,
                   ODE &ode) {
  for (int ii = 1; ii < t.size(); ++ii) {
    double dt = t[ii] - t[ii - 1];
    x.col(ii) = x.col(ii - 1) + dt * ode.fx(x.col(ii - 1), u.col(ii - 1));
  }
}

template <typename ODE> // ODE function class
void RungeKutta4(const VectorXd &t, MatrixXd &x, const MatrixXd &u, ODE &ode) {
  for (int ii = 1; ii < t.size(); ++ii) {
    double dt = t[ii] - t[ii - 1];
    Vector3d k1 = ode.fx(x.col(ii - 1), u.col(ii - 1));
    Vector3d k2 = ode.fx(x.col(ii - 1) + 0.5 * dt * k1, u.col(ii - 1));
    Vector3d k3 = ode.fx(x.col(ii - 1) + 0.5 * dt * k2, u.col(ii - 1));
    Vector3d k4 = ode.fx(x.col(ii - 1) + dt * k3, u.col(ii - 1));
    x.col(ii) = x.col(ii - 1) + (dt / 6) * (k1 + 2 * k2 + 2 * k3 + k4);
  }
}

#endif // SIMULATORS_H