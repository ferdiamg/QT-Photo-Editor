#ifndef AKKUMULATOR_H
#define AKKUMULATOR_H

class Akkumulator {
public:
  int a;
  int Umin;
  int Vmin;
  int Umax;
  int Vmax;
  Akkumulator(int aInput = 0, int UminInput = -1, int VminInput = -1,
              int UmaxInput = 2000, int VmaxInput = 2000);
};

#endif // AKKUMULATOR_H
