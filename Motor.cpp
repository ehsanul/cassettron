#include "Arduino.h"
#include "Motor.h"
#include <math.h>

Motor::Motor(byte motorPin, byte encoderPin): motorPin(motorPin), encoderPin(encoderPin)
{
}

// setup
void Motor::setup(void (*ISR_callback)(void))
{
  // get rid of annoying high whine by making the pwm frequency inaudible
  analogWriteFrequency(motorPin, 22000.0);

  // XXX we are getting different motor speeds given different analog write
  // frequencies. i know pwm frequency can affect motor speed, but the effect
  // seems too great to be explained by that. it's likely caused by a bad motor
  // driver not translating the pwm properly.
  //
  //analogWriteFrequency(motorPin, 20000.0);
  //analogWriteFrequency(motorPin, 30000.0);
  //analogWriteFrequency(motorPin, 36621.09);
  //analogWriteFrequency(motorPin, 40000.0);
  //analogWriteFrequency(motorPin, 50000.0);
  //analogWriteFrequency(motorPin, 60000.0);
  //analogWriteFrequency(motorPin, 70000.0);

  // according to paul, the hardware will provide as much resolution as possible given the
  // frequency we set: https://forum.pjrc.com/threads/66491-Pwm?p=272477&viewfull=1#post272477
  // optimal resolution/frequency pairs are noted here: https://www.pjrc.com/teensy/td_pulse.html
  analogWriteResolution(resolution);

  pinMode(encoderPin, INPUT_PULLDOWN);
  pinMode(motorPin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(encoderPin), ISR_callback, RISING);
}

// value between 0.0 and 1.0
void Motor::setSpeed(float value)
{
  analogWrite(motorPin, value * maxWriteValue);
}

void Motor::handleInterrupt()
{
  encoderCount += 1;
  diffTimeMicros = monotonicTimeMicros - lastCountTimeMicros;
  if (diffTimeMicros < avgDiffTimeMicros / 4) {
    if (skipCount < 3) {
      // skip bad value!
      skipCount += 1;
      //Serial.print("Bad: ");
      //Serial.print(diffTimeMicros);
      //Serial.print(", Avg: ");
      //Serial.print(avgDiffTimeMicros);
      //Serial.println("");
    } else {
      // reset, we skipped too many times
      avgDiffTimeMicros = 0;
    }
  } else {
    skipCount = 0;
    filteredEncoderCount += 1;
    if (avgDiffTimeMicros == 0 && lastCountTimeMicros != 0) {
      // initial value
      avgDiffTimeMicros = diffTimeMicros;
    } else if (avgDiffTimeMicros != 0) {
      avgDiffTimeMicros = EMA_a * diffTimeMicros + (1.0 - EMA_a) * avgDiffTimeMicros;
      // in case we ever get a nan here, let's recover from it
      if (isnan(avgDiffTimeMicros) || isinf(avgDiffTimeMicros)) { avgDiffTimeMicros = 0; }
    }
  }
  lastCountTimeMicros = monotonicTimeMicros;
}

void Motor::calculateFrequency() {
  float timeSeconds = (float) timeMicros / 1000000;

  float rawFrequency = (float) encoderCount / 20 / timeSeconds;
  // float filteredFrequency = (float) filteredEncoderCount / 20 / timeSeconds;

  // use the moving average and time since last count to predict a partial
  // count, in an attempt to get a smoother result at low speeds
  int microsSinceLastCount = monotonicTimeMicros - lastCountTimeMicros;
  float partialCount = (float) microsSinceLastCount / avgDiffTimeMicros;
  float filteredCountPlusPartial = (float) filteredEncoderCount + partialCount - lastPartialCount;
  lastPartialCount = partialCount;

  float filteredFrequencyPlusPartial = (float) filteredCountPlusPartial / 20 / timeSeconds;

  // moving average for final frequency
  frequency = 0.4 * filteredFrequencyPlusPartial + 0.6 * frequency;

  // in case we ever get a nan or negative value here, let's recover from it
  if (isnan(frequency) || isinf(frequency) || frequency < 0) { frequency = 0; }

  //Serial.print(",");
  //Serial.print("count:");
  //Serial.print(encoderCount);

  //Serial.print(",");
  //Serial.print("filtered_count:");
  //Serial.print(filteredEncoderCount);

  //Serial.print(",");
  //Serial.print("filtered_count_plus_partial:");
  //Serial.print(filteredCountPlusPartial);

  //Serial.print(",");
  //Serial.print("diff:");
  //Serial.print(diffTimeMicros);

  //Serial.print(",");
  Serial.print("raw_frequency:");
  Serial.print(rawFrequency);

  // Serial.print(",");
  // Serial.print("filtered_freq:");
  // Serial.print(filteredFrequency);

  // Serial.print(",");
  // Serial.print("filtered_freq_plus_partial:");
  // Serial.print(filteredFrequencyPlusPartial);

  Serial.print(",");
  Serial.print("frequency:");
  Serial.print(frequency);

  Serial.println("");

  timeMicros = 0;
  encoderCount = 0;
  filteredEncoderCount = 0;
}
