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
/*! Simulators*/
/* ---------------------------------------------------- */
// Euler forward method
template <typename ODE> // ODE function class
void euler_forward(const VectorXd &t, MatrixXd &x, const VectorXd &ref,
                   ODE &ode) {
  for (int ii = 1; ii < t.size(); ++ii) {
    double dt = t[ii] - t[ii - 1];
    x.col(ii) = x.col(ii - 1) + dt * ode.fx(x.col(ii - 1), ref[ii - 1]);
  }
}

template <typename ODE> // ODE function class
void RungeKutta4(const VectorXd &t, MatrixXd &x, const VectorXd &ref,
                 ODE &ode) {
  for (int ii = 1; ii < t.size(); ++ii) {
    double dt = t[ii] - t[ii - 1];
    VectorXd k1 = ode.fx(x.col(ii - 1), ref[ii - 1]);
    VectorXd k2 = ode.fx(x.col(ii - 1) + 0.5 * dt * k1, ref[ii - 1]);
    VectorXd k3 = ode.fx(x.col(ii - 1) + 0.5 * dt * k2, ref[ii - 1]);
    VectorXd k4 = ode.fx(x.col(ii - 1) + dt * k3, ref[ii - 1]);
    x.col(ii) = x.col(ii - 1) + (dt / 6) * (k1 + 2 * k2 + 2 * k3 + k4);
  }
}

/* Ordinary differential equation class */
class ode_fn {
public:
  double L;   // inductance
  double C;   // capacitance
  double R;   // resistance
  VectorXd t; // time vector
  MatrixXd x; // state vector

  // ODE function
  MatrixXd fx(const MatrixXd &x, const double &u) {
    MatrixXd A = MatrixXd::Zero(2, 2);
    A << 0, 1 / C, -1 / L, -R / L;
    // std::cout << "A: \n" << A << std::endl;
    return A * x;
  }

  // set time vector
  void set_time(const double &start, const double &end, const double &delta) {
    t = VectorXd::LinSpaced((end - start) / delta, start, end);
  }

  // simulate the system
  void simulate(const VectorXd &u, const VectorXd &x0) {
    x = MatrixXd::Zero(2, t.size());
    x.col(0) = x0;

    RungeKutta4(t, x, u, *this);
  }
};

/* Other functions */
VectorXd UnitStep(const VectorXd &t, const double &t0) {
  VectorXd u = VectorXd::Zero(t.size()); // initialize with zeros
  for (int ii = 0; ii < t.size(); ++ii) {
    if (t[ii] > t0) {
      u[ii] = 1;
    }
  }
  return u;
}

/*! Main function */
int main() {

  ode_fn RLC;
  RLC.L = 1e-2;
  RLC.C = 1e-2;
  RLC.R = 1e-1;

  /* ---------------------------------------------------- */
  /*! simulation */
  /* ---------------------------------------------------- */
  RLC.set_time(0, 1, 1e-3); // set time vector

  VectorXd x0 = VectorXd::Zero(2); // initialization
  x0 << -1, 0;                     // initial conductions

  VectorXd ref = UnitStep(RLC.t, 0.1); // input step function

  RLC.simulate(ref, x0); // simulate the system
  /* ---------------------------------------------------- */

  /* ---------------------------------------------------- */
  /*! saving as the data as json file */
  /* ---------------------------------------------------- */
  nljson json_obj;
  json_obj["time"] = RLC.t;
  json_obj["ref"] = ref;
  for (int ii = 0; ii < RLC.x.rows(); ++ii) {
    json_obj["ode4"]["X" + std::to_string(ii + 1)] = RLC.x.row(ii);
  }
  create_jsonfile("simple_lpf", json_obj);
  /* ---------------------------------------------------- */

  /* exit */
  return 0;
}
