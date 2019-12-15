#include "line.h"

Line::Line(float thetaInput, float rInput, int aInput, int UminInput,
           int VminInput, int UmaxInput, int VmaxInput) {
  theta = thetaInput;
  r = rInput;
  a = aInput;
  Umin = UminInput;
  Vmin = VminInput;
  Umax = UmaxInput;
  Vmax = VmaxInput;
}
