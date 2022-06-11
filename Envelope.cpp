#include "Envelope.h"
#include <math.h>

Envelope::Envelope(): t1(1000), l1(1.0), t2(1000), l2(1.0)
{
}

// value ms
void Envelope::setT1(int value)
{
  t1 = value;
}

// value between 0.0 and 1.0
void Envelope::setL1(float value)
{
  l1 = value;
}

// value ms
void Envelope::setT2(int value)
{
  t2 = value;
}

// value between 0.0 and 1.0
void Envelope::setL2(float value)
{
  l2 = value;
}

// returns level betwee 0.0 and 1.0 given time in ms
float Envelope::getLevel(int t)
{
  float l0 = 0.0;
  int t0 = 0;
  if (t <= t1) {
    return l0 + (l1 - l0) * (float) (t - t0) / t1;
  } else if (t < t1 + t2) {
      return l1 + (l2 - l1) * (float) (t - t1) / t2;
  } else {
    return l2;
  }
  }
