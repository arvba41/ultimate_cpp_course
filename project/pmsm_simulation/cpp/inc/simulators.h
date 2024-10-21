#ifndef SIMULATORS_H
#define SIMULATORS_H

#include <eigen3/Eigen/Dense>

using namespace Eigen;

namespace FixedStepSimulators {
/*! Base class for fixed-step simulators */

/* Abstract class for fixed-step simulators */
class SimulationModel {
public:
  SimulationModel() = default;
  explicit SimulationModel(size_t n_in) : n(n_in) {}
  virtual void operator()(double t, const VectorXd &x,
                          VectorXd &xdot) const = 0;
  size_t n{0};
};

/* Runge-Kutta 4th order method */
class RungeKutta {
public:
  explicit RungeKutta(const SimulationModel &model)
      : x(model.n), model(model), k1(model.n), k2(model.n), k3(model.n),
        k4(model.n), tmp(model.n) {}

  void step(const double &dt) {
    // ugly but avoids allocations
    // model(t, x, k1);
    // tmp = x + 0.5 * dt * k1;
    // model(t + 0.5 * dt, tmp, k2);
    // tmp = x + 0.5 * dt * k2;
    // model(t + 0.5 * dt, tmp, k3);
    // tmp = x + dt * k3;
    // model(t + dt, tmp, k4);

    model(t, x, k1);
    model(t + 0.5 * dt, x + 0.5 * dt * k1, k2);
    model(t + 0.5 * dt, x + 0.5 * dt * k2, k3);
    model(t + dt, x + dt * k3, k4);

    x += dt / 6 * (k1 + 2 * k2 + 2 * k3 + k4);
  }

  auto solve(const double &t0, const double &T, const VectorXd &x0, double &dt)
      -> std::pair<VectorXd, MatrixXd> {

    size_t N = std::ceil((T - t0) / dt) + 1;
    VectorXd ts = VectorXd::Zero(N);
    MatrixXd xs = MatrixXd::Zero(model.n, N);

    t = t0;
    x = x0;
    for (size_t i = 0; i < N; ++i) {
      ts(i) = t; // store time

      // std::ranges::copy(x, std::begin(xs) + i * model.n);
      xs.col(i) = x; // store state

      if ((t + dt) > T)
        dt = T - t; // adjust time step

      step(dt);
      t += dt;
    }

    return {ts, xs};
  }

private:
  VectorXd x;
  double t{0.0};

  const SimulationModel &model;

  VectorXd k1;
  VectorXd k2;
  VectorXd k3;
  VectorXd k4;
  VectorXd tmp;
};

} // namespace FixedStepSimulators

#endif // SIMULATORS_H