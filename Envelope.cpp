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
// value ms
void Envelope::setT3(int value)
{
  t3 = value;
}

// value between 0.0 and 1.0
void Envelope::setL3(float value)
{
  l3 = value;
}

void Envelope::setT4(int value)
{
  t4 = value;
}

// returns level betwee 0.0 and 1.0 given time in ms
float Envelope::holdLevel(int t)
{
  float l0 = 0.0;
  int t0 = 0;
  if (t <= t1) {
    return l0 + (l1 - l0) * (float) (t - t0) / t1;
  } else if (t < t1 + t2) {
      return l1 + (l2 - l1) * (float) (t - t1 - t0) / t2;
  }else if (t < t1 + t2 +t3) {
      return l2 + (l3 - l2) * (float) (t - t2 - t1 - t0) / t3;
  } else {
    return l3;
  }
}
float Envelope::releaseLevel(int t)
{
    float l4 = 0.0;
    float level = l3 + (l4 - l3) * (float) t / t4;
    if (level > 0.0) {
      return level;
    }else{
      return 0.0;
   }
}
