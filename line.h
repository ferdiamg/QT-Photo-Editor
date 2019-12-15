#ifndef LINE_H
#define LINE_H

class Line {
public:
  float theta;
  float r;
  int a;
  int Umin;
  int Vmin;
  int Umax;
  int Vmax;
  Line(float thetaInput, float rInput, int aInput, int UminInput, int VminInput,
       int UmaxInput, int VmaxInput);
};

#endif // LINE_H
