#include <cmath>
#include <eigen3/Eigen/Dense>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

// #include <valarray>

using namespace Eigen;
using nljson = nlohmann::json;

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

class low_pass_filter {
public:
  double alpha;

  // ode for the low pass filter
  double f_x(double &x, double &u) { return alpha * (u - x); }
};

void euler_forward(VectorXd &t, VectorXd &x, VectorXd &ref,
                   low_pass_filter &lpf) {
  x[0] = 0;
  for (int ii = 1; ii < t.size(); ++ii) {
    x[ii] = x[ii - 1] + (t[ii] - t[ii - 1]) * lpf.f_x(x[ii - 1], ref[ii - 1]);
  }
}

int main() {

  low_pass_filter lpf;
  lpf.alpha = 1 / 0.1;

  int N = 1000;

  VectorXd t = VectorXd::LinSpaced(N, 0, 1);

  // Creating a step function
  VectorXd ref = VectorXd::Zero(N);
  for (int ii = 0; ii < N; ++ii) {
    if (ii > int(N / 5)) {
      ref[ii] = 1;
    } else {
      ref[ii] = 0;
    }
  }

  // Euler forward for loop test
  VectorXd x = VectorXd::Zero(t.size());
  euler_forward(t, x, ref, lpf);

  // saving as a json file
  nljson json_obj;
  json_obj["time"] = t;
  json_obj["ref"] = ref;
  json_obj["f(y)"]["ode1"] = x;
  create_jsonfile("simple_lpf", json_obj);

  return 0;
}

// The ode function A * x + B * u
// A * x + B * u