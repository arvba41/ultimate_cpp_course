#include <cmath>
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
  PMSM() : FixedStepSimulators::SimulationModel(6) {}
  void operator()(double t, const VectorXd &x, VectorXd &xdot) const override {
    // inputs -- omega_ref, Tl
    double Tl = (t > 0.5) ? 0.08 : 0.0;

    // states -- id, iq, omega, Id, Iq
    double id = x(0);
    double iq = x(1);
    double omega = x(2);

    // controller
    static double vd, vq;
    static int ncount = 0;
    if (t > (ncount * Tc)) {
      controller(t, x, xdot, vd, vq);
      ncount++;
    } else {
      xdot(3) = 0.0;
      xdot(4) = 0.0;
      xdot(5) = 0.0;
    }

    // state update
    xdot(0) = 1 / Ld * (vd - Rs * id + np * omega * Lq * iq);
    xdot(1) = 1 / Lq * (vq - Rs * iq - np * omega * (Ld * id + psi_r));
    xdot(2) =
        1 / J *
        (3 * np / 2 * (psi_r * iq + (Ld - Lq) * id * iq) - Tl - b * omega);
  }
  double Ld{375E-6};    // d-axis inductance
  double Lq{435e-6};    // q-axis inductance
  double Rs{0.56};      // stator resistance
  double psi_r{0.0143}; // rotor flux
  double np{2};         // pole pairs
  double J{0.12e-4};    // inertia
  double b{0.01e-3};    // friction

  // PMSM Limits
  double Vbase{12.0}; // base voltage
  double Ibase{2.0};  // base current

  // controller sample time and simulator sample time
  double Tc{2E-4}; // current controller time constant

  // controller parameters
  double tri_c{1E-3};                       // current controller time constant
  double alpha_c = std::log(9) / tri_c;     // closed-loop bandwidth
  double Kpd = Ld * alpha_c;                // d-axis proportional gain
  double Kpq = Lq * alpha_c;                // q-axis proportional gain
  double Kid = Ld * alpha_c * alpha_c;      // d-axis integral gain
  double Kiq = Lq * alpha_c * alpha_c;      // q-axis integral gain
  double Rad = Ld * alpha_c - Rs;           // d-axis active resistance
  double Raq = Lq * alpha_c - Rs;           // q-axis active resistance
  double tri_s = 1E-2;                      // speed controller time constant
  double alpha_s = std::log(9) / tri_s;     // speed controller bandwidth
  double psi = 3 * np * psi_r / 2;          // flux linkage
  double Kps = J * alpha_s / psi;           // speed proportional gain
  double Kis = J * alpha_s * alpha_s / psi; // speed integral gain
  double Ba = (alpha_s * J - b) / psi;      // speed damping

  // controller setpoints
  double omega_set = 2000 * M_PI / 30; // 2000 rpm

  // adding a function that performs the control
  void controller(const double &t, const VectorXd &x, VectorXd &xdot,
                  double &vd, double &vq) const {
    // inputs -- omega_ref
    double omega_ref = (t > 0.1) ? omega_set : 0.0;
    // double Tl = (t > 0.5) ? 0.08 : 0.0;

    // states -- id, iq, omega, Id, Iq
    double id = x(0);
    double iq = x(1);
    double omega = x(2);
    double Id = x(3); // controller integrator state
    double Iq = x(4); // controller integrator state
    double Is = x(5); // controller integrator state

    // controller
    double idref = 0.0;
    double iqref_ = Kps * (omega_ref - omega) + Kis * Is - Ba * omega;

    // Saturation
    double iqref = (iqref_ > Ibase) ? Ibase : iqref_; // saturation function

    // controller
    double vdref =
        Kpd * (idref - id) + Kid * Id - Rad * id - np * omega * Lq * iq;
    double vqref = Kpq * (iqref - iq) + Kiq * Iq - Raq * iq +
                   np * omega * (Ld * id + psi_r);

    // Saturation
    double Vdq = std::sqrt(vdref * vdref + vqref * vqref);
    double sat_fcn = ((Vdq > Vbase) ? Vbase / Vdq : 1.0);
    vd = vdref * sat_fcn;
    vq = vqref * sat_fcn;

    // controller integrator update
    xdot(3) = ((idref - id) + (1 / Kpd) * (vd - vdref)) / 1e-6 * Tc;
    xdot(4) = ((iqref - iq) + (1 / Kpq) * (vq - vqref)) / 1e-6 * Tc;
    xdot(5) = ((omega_ref - omega) + (1 / Kps) * (iqref - iqref_)) / 1e-6 * Tc;
  }
};

/*! Main function */
int main() {

  // user defined parameters
  double t0 = 0.0;  // start time
  double T = 1.0;   // end time
  double dt = 1E-6; // time step

  VectorXd x0 = VectorXd::Zero(6); // initial state

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

  create_jsonfile("pmsm_sim_cpp", json_obj); // save to json file

  fmt::print("Number of allocations: {}\n", n_allocations);

  return 0;
}
