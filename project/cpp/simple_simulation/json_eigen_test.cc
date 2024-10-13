#include <cmath>
#include <eigen3/Eigen/Dense>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

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

int main() {

  double f1 = 50.0; // frequency
  double ma = 0.9;  // modulation amplitude

  // definin a time array
  VectorXd t = VectorXd::LinSpaced(16, 0, 1 / f1).transpose();

  // std::cout << "Time array: \n" << 2.0 * M_PI * t << std::endl;

  // defining a sine and cos wave
  MatrixXd waveform(2, t.size());
  waveform.row(0) = ma * sin(2.0 * M_PI * f1 * t.array());
  waveform.row(1) = ma * cos(2.0 * M_PI * f1 * t.array());

  // printing the sine wave
  // std::cout << "Sine wave: \n" << waveform << std::endl;

  // creating a json object and saving it to a file
  nljson json_obj;
  json_obj["time"] = t;
  json_obj["f(y)"]["sin"] = waveform.row(0);
  json_obj["f(y)"]["cos"] = waveform.row(1);
  create_jsonfile("sine_cos_wave", json_obj);

  return 0;
}