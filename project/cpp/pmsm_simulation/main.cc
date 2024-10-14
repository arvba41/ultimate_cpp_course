#include <eigen3/Eigen/Dense>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

// #include <valarray>

using namespace Eigen;
using nljson = nlohmann::json;

/* ---------------------------------------------------- */
/* Function to reate a json file */
/* ---------------------------------------------------- */
void create_jsonfile(std::string filename, const nljson &json_obj) {

  filename += ".json"; // Append .json to filename

  std::ofstream file(filename);
  if (file.is_open()) {
    file << json_obj.dump(4); // Pretty print with 4 spaces
    file.close();
    std::cout << "saved to " << filename << std::endl;
  } else {
    std::cerr << "Unable to open file for writing" << std::endl;
  }
}
/* ---------------------------------------------------- */

/* ---------------------------------------------------- */
/*! Simulators*/
/* ---------------------------------------------------- */
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
    VectorXd k1 = ode.fx(x.col(ii - 1), u.col(ii - 1));
    VectorXd k2 = ode.fx(x.col(ii - 1) + 0.5 * dt * k1, u.col(ii - 1));
    VectorXd k3 = ode.fx(x.col(ii - 1) + 0.5 * dt * k2, u.col(ii - 1));
    VectorXd k4 = ode.fx(x.col(ii - 1) + dt * k3, u.col(ii - 1));
    x.col(ii) = x.col(ii - 1) + (dt / 6) * (k1 + 2 * k2 + 2 * k3 + k4);
  }
}
/* ---------------------------------------------------- */

/* ---------------------------------------------------- */
/* electric machine class */
/* ---------------------------------------------------- */
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

void PMSM::simulate(const VectorXd &x0) { // simulate the PMSM
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
/* ---------------------------------------------------- */

/*! Main function */
int main() {

  PMSM pmsm; // create an instance of the PMSM class
  // set the parameters
  pmsm.Ld = 2.85E-3;   // d-axis inductance
  pmsm.Lq = 2.85E-3;   // q-axis inductance
  pmsm.Rs = 0.054;     // stator resistance
  pmsm.psi_r = 0.8603; // rotor flux
  pmsm.np = 3;         // pole pairs
  pmsm.J = 0.25;       // inertia

  const double Vd_pk = 100; // peak value of the d-axis voltage
  const double tset = 0.1;  // time to set the input
  const double tstart = 0;  // simulation start time
  const double tend = 1;    // simulation end time
  const double dt = 10E-6;  // simulation time step

  /* ---------------------------------------------------- */
  /*! simulation */
  /* ---------------------------------------------------- */
  pmsm.set_time(tstart, tend, dt); // set time vector
  pmsm.set_inputs(1, Vd_pk, tset); // set input vector
  VectorXd x0 = VectorXd::Zero(3); // initial state vector
  pmsm.simulate(x0);               // simulate the system
  /* ---------------------------------------------------- */

  /* ---------------------------------------------------- */
  /*! saving as the data as json file */
  /* ---------------------------------------------------- */
  nljson json_obj;
  json_obj["t"] = pmsm.t;
  for (int ii = 0; ii < pmsm.u.rows(); ++ii) {
    json_obj["U"][std::to_string(ii)] = pmsm.u.row(ii);
  }
  for (int ii = 0; ii < pmsm.x.rows(); ++ii) {
    json_obj["X"][std::to_string(ii)] = pmsm.x.row(ii);
  }
  create_jsonfile("pmsm_simulation", json_obj);
  /* ---------------------------------------------------- */

  /* exit */
  return 0;
}
