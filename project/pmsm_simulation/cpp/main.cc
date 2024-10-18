#include <eigen3/Eigen/Dense>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

// #include "pmsm.h"  // PMSM class
#include "simulators.h" // Fixed-step simulators
#include "timer.h"      // Timer class

using namespace Eigen;
using nljson = nlohmann::json;

// Raises a warning when using gcc compiler use clang instead. If you still want
// to use gcc, you can suppress the warning by adding
// -Wno-deprecated-declarations to the compiler flags.
static int n_allocations = 0;
void *operator new(size_t size) {
  ++n_allocations;
  return malloc(size);
}

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
/* PMSM as a struct */
/* ---------------------------------------------------- */
struct PMSM : public FixedStepSimulators::SimulationModel {
  PMSM() : FixedStepSimulators::SimulationModel(3) {}
  void operator()(double t, const Vector3d &x, Vector3d &xdot) const override {
    double vd = 0;
    double vq = (t > 0.1) ? 100.0 : 0.0;
    double Tl = 0;

    double id = x(0);
    double iq = x(1);
    double omega = x(2);

    xdot(0) = 1 / Ld * (vd - Rs * id + np * omega * Lq * iq);
    xdot(1) = 1 / Lq * (vq - Rs * iq - np * omega * (Ld * id + psi_r));
    xdot(2) = 1 / J * (3 * np / 2 * (psi_r * iq + (Ld - Lq) * id * iq) - Tl);
  }
  double Ld{2.85E-3};   // d-axis inductance
  double Lq{2.85E-3};   // q-axis inductance
  double Rs{0.054};     // stator resistance
  double psi_r{0.8603}; // rotor flux
  double np{3};         // pole pairs
  double J{0.25};       // inertia
};

/*! Main function */
int main() {

  // user defined parameters
  double t0 = 0.0;  // start time
  double T = 1.0;   // end time
  double dt = 1E-6; // time step

  Vector3d x0;
  x0 << 0.0, 0.0, 0.0; // initial state

  // create PMSM object
  const PMSM pmsm;
  FixedStepSimulators::RungeKutta rk(pmsm);

  Timer timer;
  timer.tic();                             // start timer
  auto [ts, xs] = rk.solve(t0, T, x0, dt); // simulate
  timer.toc();                             // stop timer

  fmt::print("Simulated {} data points in {} ms\n", ts.size(), timer.elapsed());

  // create json object
  nljson json_obj;
  json_obj["t"] = ts;
  for (size_t i = 0; i < pmsm.n; ++i) {
    json_obj["x" + std::to_string(i)] = xs.row(i);
  }
  // json_obj["x"] = xs;

  create_jsonfile("pmsm_sim_cpp", json_obj); // save to json file

  fmt::print("Number of allocations: {}\n", n_allocations);

  return 0;
}
