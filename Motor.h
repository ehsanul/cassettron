#ifndef Motor_h
#define Motor_h

#include "Arduino.h"
#include <math.h>

class Motor
{
  public:
    Motor(byte motorPin, byte encoderPin);
    void setup(void (*ISR_callback)(void));
    void setSpeed(float value);
    void handleInterrupt();
    void calculateFrequency();
  private:
    static constexpr float EMA_a = 0.4;
    float frequency;
    int motorPin;
    int encoderPin;
    int resolution = 12;
    int maxWriteValue = pow(2, resolution);

    elapsedMicros monotonicTimeMicros;
    elapsedMicros timeMicros;
    volatile int encoderCount = 0;
    volatile int filteredEncoderCount = 0;
    volatile int skipCount = 0;
    volatile int lastCountTimeMicros = 0;
    volatile int diffTimeMicros = 0;
    volatile float avgDiffTimeMicros = 0;
    float lastPartialCount = 0;
};

#endif
