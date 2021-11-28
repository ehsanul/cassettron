#include "CassetronMotor.h"
#include <EEPROM.h>

const byte encoderPin1 = 33;
const byte motorPin1 = 1;
elapsedMillis timeMillis;

Motor motor1(motorPin1, encoderPin1);
const double TET12 = 1.05946309436; // 12th root of 2`

const int MIN_NOTE = 24; // lowest note we can reach, TODO retune when middleC is tuned
const int MAX_NOTE = 78; // highest note we can reach, TODO retune when middleC is tuned
int currentNote = MIN_NOTE;
float pidValues[MAX_NOTE - MIN_NOTE +1];
const int pidValueHistoryLength = 10;
float pidValueHistory[pidValueHistoryLength];
int pidValueHistoryIndex = 0;

const int frequencyHistoryLength = 20;
float frequencyHistory[frequencyHistoryLength];
int frequencyHistoryIndex = 0;
int numSamples = 0;

const float THRESHOLD_ERROR_PERCENT = 2.5; // TODO tune this value
const float THRESHOLD_DEVIATION = 0.1; // TODO tune this value

void setup() {
  Serial.begin(115200);

  motor1.setup([]{motor1.handleInterrupt();});
  setMotorFrequency(MIN_NOTE);
}

void loop() {
  // every 50ms, do motor control actions
  if (timeMillis > 50) {
    timeMillis = 0;
    motor1.step();

    addPidValueHistory(motor1.pidValue());
    addFrequencyHistory(motor1.frequency);
    numSamples += 1;

    // if the motor has been running at a stable speed for long enough, we can
    // store the average value for analog writes to it
    bool fullySampled = numSamples >= frequencyHistoryLength;
    float errorPercent = 100.0 * (avgFrequency() - motor1.desiredFrequency) / motor1.desiredFrequency;
    bool matches = abs(errorPercent) < THRESHOLD_ERROR_PERCENT;
    bool stableSpeed = frequencyDeviation() < THRESHOLD_DEVIATION;
    // TODO check for stablePidValue
    // TODO check that frequency actually matches what we expect!
    // TODO handle situation where we can't reach the note (max note set higher than we can actually hit)
    if (fullySampled && matches && stableSpeed) {
      Serial.print("got stable speed for note: ");
      Serial.println(currentNote);
      addPidValue(currentNote, avgPidValue());
      if (currentNote < MAX_NOTE) {
        // go to next note!
        currentNote += 1;
        numSamples = 0;
        setMotorFrequency(currentNote);
      } else {
        Serial.print("done!");
        // if we are at the max note, we are done!
        EEPROM.put(0, pidValues);
        exit(0);
      }
    }
  }
}

float avgPidValue(){
  float sum = 0;
  for(int i = 0; i < pidValueHistoryLength; i++){
    sum += pidValueHistory[i];
  }
  return sum / pidValueHistoryLength;
}

float avgFrequency(){
  float sum = 0;
  for(int i = 0; i < frequencyHistoryLength; i++){
    sum += frequencyHistory[i];
  }
  return sum / frequencyHistoryLength;
}

// numerator in std dev formula
float frequencyDeviation(){
  float sum = 0;
  int deviation;
  int mean = avgFrequency();
  for(int i = 0; i < frequencyHistoryLength; i++){
    deviation = frequencyHistory[i] - mean;
    sum += deviation * deviation;
  }
  return sum / frequencyHistoryLength;
}

void addPidValue(int note, float pidValue) {
  int index = note - MIN_NOTE;
  pidValues[index] = pidValue;
}

void addPidValueHistory(float pidValue) {
    pidValueHistory[pidValueHistoryIndex] = pidValue;
    pidValueHistoryIndex += 1;
    pidValueHistoryIndex %= pidValueHistoryLength; // wraparound
}

void addFrequencyHistory(float frequency) {
  frequencyHistory[frequencyHistoryIndex] = frequency;
  frequencyHistoryIndex += 1;
  frequencyHistoryIndex %= frequencyHistoryLength;
}

// TODO move to Voice class
void setMotorFrequency(int note) {
  int middleC = 60;
  double factor = pow(TET12, abs(note - middleC));
  double desiredFrequency = 7.5; // supposed to be middleC, but not actually tuned!!!
  if (note >= middleC) {
    desiredFrequency *= factor;
  } else {
    desiredFrequency /= factor;
  }
  Serial.print("setting desired frequency to: ");
  Serial.println(desiredFrequency);
  motor1.desiredFrequency = (float) desiredFrequency;
}
