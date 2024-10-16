#include <eigen3/Eigen/Dense>
#include <iostream>

using namespace Eigen;

class EulerForward {
public:
  double dt;
  double x;
  double u;
  auto f; // function pointer

  //   double f_x(double x, double u) { return u - x; }

  void euler_forward() {
    x = 0;
    for (int ii = 1; ii < 10; ++ii) {
      x = x + dt * f_x(x, u);
    }
  }
};