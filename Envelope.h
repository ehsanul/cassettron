#ifndef Envelope_h
#define Envelope_h

#include <math.h>

class Envelope
{
  public:
    Envelope();
    void setT1(int value); //ms
    void setL1(float value);
        void setT2(int value); //ms
    void setL2(float value);
    float getLevel(int time); //returns 0.0 - 1.0
  private:
    int t1; // ms
    int t2; // ms
    float l1; // 0.0 - 1.0
    float l2;
};

#endif
