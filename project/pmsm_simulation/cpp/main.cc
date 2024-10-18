#include <eigen3/Eigen/Dense>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "pmsm.h"  // PMSM class
#include "timer.h" // Timer class

using namespace Eigen;
using nljson = nlohmann::json;

// Does not work with g++ compiler for some reason
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

/*! Main function */
int main() {

  // set the parameters
  double Ld = 2.85E-3;   // d-axis inductance
  double Lq = 2.85E-3;   // q-axis inductance
  double Rs = 0.054;     // stator resistance
  double psi_r = 0.8603; // rotor flux
  int np = 3;            // pole pairs
  double J = 0.25;       // inertia

  const double Vd_pk = 100; // peak value of the d-axis voltage
  const double tset = 0.1;  // time to set the input
  const double tstart = 0;  // simulation start time
  const double tend = 1;    // simulation end time
  const double dt = 1E-6;   // simulation time step

  /* ---------------------------------------------------- */
  /*! simulation */
  /* ---------------------------------------------------- */
  PMSM pmsm(Ld, Lq, Rs, psi_r, np, J); // create PMSM object

  pmsm.set_time(tstart, tend, dt); // set time vector
  pmsm.set_inputs(1, Vd_pk, tset); // set input vector
  Vector3d x0 = VectorXd::Zero(3); // initial state vector

  Timer timer; // initialize timer

  timer.tic();   // start timer
  timer.reset(); // start timer

  pmsm.simulate(x0); // simulate the system

  timer.toc(); // stop timer

  std::cout << "Elapsed time: " << timer.elapsed() << " ms" << std::endl;
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

  // Print the number of allocations
  std::cout << "Number of allocations: " << n_allocations << std::endl;

  /* exit */
  return 0;
}
