#ifndef Motor_h
#define Motor_h

#include "Arduino.h"
#include <math.h>

class Motor
{
  public:
    Motor(byte motorPin, byte encoderPin);
    void setup(void (*ISR_callback)(void));
    void handleInterrupt();
    void step();
    void setDesiredFrequency(float value);
    float pidValue();
    float frequency;
  private:
    void setSpeed(float value);
    void calculateFrequency();
    static constexpr float EMA_a = 0.4;
    float desiredFrequency = 4.0;
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
    volatile int lastPidTime = 0;
    volatile int diffTimeMicros = 0;
    volatile float avgDiffTimeMicros = 0;
    float lastPartialCount = 0;
    float lastError = 0;
    float intError = 0;
    float lastIntError = 0;
};

#endif
