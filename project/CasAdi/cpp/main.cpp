/*
 *    MIT No Attribution
 *
 *    Copyright (C) 2010-2023 Joel Andersson, Joris Gillis, Moritz Diehl, KU
 * Leuven.
 *
 *    Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <casadi/casadi.hpp>
#include <ctime>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

using namespace casadi;
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
// define model struct
struct vehicle {
  double Xfin = 50;  // final position
  double mass = 500; // mass of the car
  double mu = 0.8;   // friction coefficient
  double g = 9.81;   // gravity
};

// constraints
struct constraints {
  double Ymax = 5;              // y direction upper bound
  double delta_max = M_PI / 2;  // steering angle upper bound
  double ddelta_max = M_PI / 6; // steering rate upper bound
  double vx_init = 40 / 3.6;    // initial velocity
};

// obstacle struct
struct obstacle {
  double Xa = 50; // obstacle position
  double R1 = 4;  // obstacle radius
  double R2 = 2;  // obstacle radius
  double pow = 6; // sharpness of the obstacle
};

// dx/dt = f(x,u)
MX f(const MX &x, const MX &u, const vehicle &car) {
  return vertcat(x(2), x(3), u(0) / car.mass, u(1) / car.mass);
}

// obstacle function
MX obst_elipse(const MX &x, const MX &y, const obstacle &obs) {
  return 1 - pow((x - obs.Xa) / obs.R1, obs.pow) - pow(y / obs.R2, obs.pow);
}

int main() {

  // Car race along a track
  // ----------------------
  // An optimal control problem (OCP),
  // solved with direct multiple-shooting.
  //
  // For more information see: http://labs.casadi.org/OCP

  vehicle car;
  obstacle obst;
  constraints cons;

  int N = 40; // number of control intervals

  auto opti = casadi::Opti(); // Optimization problem

  Slice all;
  // ---- decision variables ---------
  auto X = opti.variable(4, N + 1); // state trajectory
  auto xpos = X(0, all);
  auto ypos = X(1, all);
  auto xspeed = X(2, all);
  auto yspeed = X(3, all);
  auto U = opti.variable(2, N); //% control trajectory(throttle)
  auto T = opti.variable();     // final time

  // ---- objective          ---------
  opti.minimize(T); // race in minimal time

  // ---- dynamic constraints --------
  auto dt = T / N;
  for (int k = 0; k < N; ++k) {
    auto k1 = f(X(all, k), U(all, k), car);
    auto k2 = f(X(all, k) + dt / 2 * k1, U(all, k), car);
    auto k3 = f(X(all, k) + dt / 2 * k2, U(all, k), car);
    auto k4 = f(X(all, k) + dt * k3, U(all, k), car);
    auto x_next = X(all, k) + dt / 6 * (k1 + 2 * k2 + 2 * k3 + k4);
    opti.subject_to(X(all, k + 1) == x_next); // close the gaps
  }

  // ---- input constraints -----------
  auto Flim =
      U(0, all) * U(0, all) + U(1, all) * U(1, all); // control force limit
  opti.subject_to(0 <= Flim <=
                  pow(car.mass * car.mu * car.g, 2)); // control force limit

  // ---- path constraints -----------
  opti.subject_to(obst_elipse(xpos, ypos, obst) <= 0); // obstacle

  // ---- spped and position constraints -----------
  opti.subject_to(xspeed > 0);            // speed is positive
  opti.subject_to(0 <= ypos < cons.Ymax); // track limits

  // ---- boundary conditions --------
  opti.subject_to(xpos(0) == 0);              // start at position 0 ...
  opti.subject_to(ypos(0) == 1);              // start at position 1 ...
  opti.subject_to(xspeed(0) == cons.vx_init); // start at speed 40/3.6 ...
  opti.subject_to(yspeed(0) == 0);            // ... from stand-still

  opti.subject_to(xpos(N) == 2 * car.Xfin); // finish line at position 1
  opti.subject_to(ypos(N) == ypos(0));      // finish line at position 1

  // ---- misc. constraints  ----------
  opti.subject_to(T >= 0); // Time must be positive

  // ---- initial values for solver ---
  opti.set_initial(xpos, 0);
  opti.set_initial(ypos, 1);
  opti.set_initial(xspeed, cons.vx_init);
  opti.set_initial(yspeed, 0);
  opti.set_initial(T, 1);
  opti.set_initial(U(0, all), 1000);
  opti.set_initial(U(1, all), 0);

  Dict opts;
  opts["ipopt.print_level"] = 0;
  // opts["ipopt.tol"] = 1e-8;
  opts["ipopt.linear_solver"] = "ma57";
  opti.solver("ipopt", opts); // set numerical backend
  // opti.solver("ipopt");    // set numerical backend
  auto sol = opti.solve(); // actual solve

  // ---- post-processing          ----
  double Tend = (double)sol.value(T);
  int Nsmp = N;

  std::vector<double> x_pos = std::vector<double>(sol.value(xpos));
  std::vector<double> y_pos = std::vector<double>(sol.value(ypos));
  std::vector<double> x_speed = std::vector<double>(sol.value(xspeed));
  std::vector<double> y_speed = std::vector<double>(sol.value(yspeed));

  std::vector<double> U0 = std::vector<double>(sol.value(U(0, all)));
  std::vector<double> U1 = std::vector<double>(sol.value(U(1, all)));

  // ---- saving results to json file ----
  nljson json_obj;
  json_obj["T"] = Tend;
  json_obj["Nsmp"] = Nsmp;
  json_obj["x_pos"] = x_pos;
  json_obj["y_pos"] = y_pos;
  json_obj["x_speed"] = x_speed;
  json_obj["y_speed"] = y_speed;
  json_obj["U0"] = U0;
  json_obj["U1"] = U1;

  create_jsonfile("casadi_cpp_sim", json_obj);

  // // Create Matlab script to plot the solution
  // std::ofstream file;
  // std::string filename = "race_car_results.m";
  // file.open(filename.c_str());
  // file << "% Results file from " __FILE__ << std::endl;
  // file << "% Generated " __DATE__ " at " __TIME__ << std::endl;
  // file << std::endl;

  // // Save results to file
  // file << "t = linspace(0," << sol.value(T) << "," << N << "+1);" <<
  // std::endl; file << "speed = " << std::vector<double>(sol.value(speed)) <<
  // ";"
  //      << std::endl;
  // file << "pos = " << std::vector<double>(sol.value(pos)) << ";" <<
  // std::endl; file << "U = " << std::vector<double>(sol.value(U)) << ";" <<
  // std::endl;

  // file << "figure;" << std::endl;
  // file << "hold on;" << std::endl;
  // file << "plot(t,speed);" << std::endl;
  // file << "plot(t,pos);" << std::endl;
  // file << "plot(t,1-sin(2*pi*pos)/2,'r--');" << std::endl;
  // file << "stairs(t(1:end-1),U,'k');" << std::endl;
  // file << "xlabel('Time [s]');" << std::endl;
  // file << "legend('speed','pos','speed "
  //         "limit','throttle','Location','northwest');"
  //      << std::endl;

  // // Have a look at the constraint Jacobian
  // jacobian(opti.g(), opti.x()).sparsity().spy_matlab("race_car_jac_g.m");

  // file << "figure" << std::endl;
  // file << "race_car_jac_g;" << std::endl;
  // file << "xlabel('decision variables');" << std::endl;
  // file << "ylabel('constraints');" << std::endl;
  // file << "print('jac_sp','-dpng');" << std::endl;

  return 0;
}