#include <Eigen/Dense>
#include <cmath>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <valarray>

using nljson = nlohmann::json;
using Vec = std::valarray<double>;

/* Function definitions */
Vec linspace(double start, double end, int num) {
  Vec vec(num);
  double step = (end - start) / (num - 1);
  for (int i = 0; i < num; i++) {
    vec[i] = start + i * step;
  }
  return vec;
}

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

/* Main function */
int main() {

  // creating a linspace vector

  auto f1 = 50;  // frequency
  auto ma = 0.9; // modulation amplitude
  Vec t = linspace(0.0, 1 / f1, 100);
  Vec y = ma * sin(2.0 * M_PI * f1 * t);

  // creating a json object and saving it to a file
  nljson json_obj;
  json_obj["time"] = t;
  json_obj["amplitude"] = y;
  create_jsonfile("sine_wave", json_obj);

  return 0;
}
