#include "Motor.h"

const byte encoderPin1 = 33;
const byte motorPin1 = 1;

Motor motor1(motorPin1, encoderPin1);
const double TET12 = 1.05946309436; // 12th root of 2`

const int MIN_NOTE = 38; // D2, lowest note we can reach
const int MAX_NOTE = 74; // D5, highest note we can reach
int currentNote = MIN_NOTE;
float* pidValues = new float[MAX_NOTE - MIN_NOTE +1];
const int pidValueHistoryLength = 5;
float* pidValueHistory = new float[pidValueHistoryLength];
int pidValueHistoryIndex = 0;

int frequencyHistoryLength = 20;
float* frequencyHistory = new float[frequencyHistoryLength];
int frequencyHistoryIndex = 0;

const float THRESHOLD_DEVIATION = 0; // TODO tune this value

void setup() {
  Serial.begin(115200);

  motor1.setup([]{motor1.handleInterrupt();});
}

void loop() {
  // every 50ms, do motor control actions
  if (timeMillis > 50) {
    timeMillis = 0;
    motor1.step();

    addPidValueHistory(motor1.pidValue()) {
    addFrequencyHistory(motor1.frequency);

    // if the motor has been running at a stable speed for long enough, we can
    // store the average value for analog writes to it
    bool stableSpeed = frequencyDeviation() < THRESHOLD_DEVIATION;
    if (stableSpeed) {
      addPidValue(currentNote, avgPidValue())
      if (currentNote < MAX_NOTE) {
        // go to next note!
        currentNote += 1;
        setMotorFrequency(currentNote);
      } else {
        // if we are at the max note, we are done!
        // TODO we need to store the pidValues to EEPROM!
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
  int mean = avgFrequency()
  for(int i = 0; i < frequencyHistoryLength; i++){
    deviation = frequencyHistory[i] - mean
    sum += deviation * deviation;
  }
  return sum / frequencyHistoryLength;
}

void addPidValue(int note, float pidValue) {
  int index = note - MIN_NOTE;
  pidValues[index] = pidValue;
}

void addPidValueHistory(float pidValue) {
    pidValueHistory[pidValueHistoryIndex] = pidValue
    pidValueHistoryIndex += 1;
    pidValueHistoryIndex %= pidValueHistoryLength; // wraparound
}

void addFrequencyHistory(float frequency) {
  frequencyHistory[frequencyHistoryIndex] = frequency;
  frequencyHistoryIndex += 1;
  frequencyHistoryIndex %= frequencyHistoryLength;
}

// TODO move to Voice class
void setMotorFrequency(note) {
  int middleC = 60;
  double factor = pow(TET12, abs(note - middleC));
  double desiredFrequency = 5.0; // supposed to be middleC, but not actually tuned!!!
  if (note >= middleC) {
    desiredFrequency *= factor;
  } else {
    desiredFrequency /= factor;
  }
  motor1.setDesiredFrequency((float) desiredFrequency);
}
