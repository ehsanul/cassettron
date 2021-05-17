#include "Motor.h"

const byte encoderPin1 = 33;
const byte motorPin1 = 1;

Motor motor1(motorPin1, encoderPin1);
const double TET12 = 1.05946309436; // 12th root of 2`

const int MIN_NOTE = 38; // D2, lowest note we can reach
const int MAX_NOTE = 74; // D5, highest note we can reach
int currentNote = MIN_NOTE;
float avgPidValue = 0.0;

void setup() {
  Serial.begin(115200);

  motor1.setup([]{motor1.handleInterrupt();});
}

void loop() {
  // every 50ms, do motor control actions
  if (timeMillis > 50) {
    timeMillis = 0;
    motor1.step();
  }

  // TODO assign avgPidValue

  // if the motor has been running at a stable speed for long enough, we can
  // store the average value for analog writes to it
  bool stableSpeed = false; // TODO implement this
  if (stableSpeed) {
    storeValue(currentNote, avgPidValue)
    if (currentNote < MAX_NOTE) {
      // go to next note!
      currentNote += 1;
      setMotorFrequency(currentNote);
    } else {
      // if we are at the max note, we are done!
      exit(0)
    }
  }
}

void storeValue(int note, float pidValue) {
  // TODO implement
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
