#include "Motor.h"

const byte encoderPin1 = 33;
const byte motorPin1 = 1;

Motor motor1(motorPin1, encoderPin1);
const double TET12 = 1.05946309436; // 12th root of 2`

const int MIN_NOTE = 38; // D2, lowest note we can reach
const int MAX_NOTE = 74; // D5, highest note we can reach
int currentNote = MIN_NOTE;
float* pidValues = new float[MAX_NOTE - MIN_NOTE +1];
const int historyLength = 5;
float* pidValueHistory = new float[historyLength];
int pidValueHistoryIndex = 0;

void setup() {
  Serial.begin(115200);

  motor1.setup([]{motor1.handleInterrupt();});
}

void loop() {
  // every 50ms, do motor control actions
  if (timeMillis > 50) {
    timeMillis = 0;
    motor1.step();

    // store pid values in pidhistory array
    pidValueHistory[pidValueHistoryIndex] = motor1.pidValue();
    if (pidValueHistoryIndex < historyLength - 1) {
      pidValueHistoryIndex += 1;
    } else {
      // we've reached the end of the array when index is historyLength-1
      // overwrite old values in array to get most recent 5 pid values
      pidValueHistoryIndex = 0;
    }
  }

  // if the motor has been running at a stable speed for long enough, we can
  // store the average value for analog writes to it
  bool stableSpeed = false; // TODO implement this
  if (stableSpeed) {
    addPidValue(currentNote, calculateAvgPidValue())
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

//calc avg of the 5 most recenet PID values by iterating over history array
float calculateAvgPidValue(){
  float sum = 0;
  for(int i=0; i<historyLength; i++){
    sum+=pidValueHistory[i];
  }
  return sum/historyLength;
}

void addPidValue(int note, float pidValue) {
  // TODO implement
  int index = note - MIN_NOTE;
  pidValues[index] = pidValue;
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
