#include "Motor.h"

elapsedMillis timeMillis;

int speedPotPin = 24;
int freqPotPin = 25;

int motorPin1 = 8;
int motorPin2 = 9;
int motorPinPWM = 1;

const byte encoderPin1 = 33;
int fakeMotorPinPWM = 2;
Motor encoderMotor(fakeMotorPinPWM, encoderPin1);

void setup() {
  Serial.begin(115200);

  pinMode(freqPotPin, INPUT);
  pinMode(speedPotPin, INPUT);

  pinMode(motorPin1 , OUTPUT);
  pinMode(motorPin2 , OUTPUT);
  pinMode(motorPinPWM , OUTPUT);
  analogWriteResolution(12); // max 4096

  encoderMotor.setup([]{encoderMotor.handleInterrupt();});
}

void loop() {
  float writeFreq = map(analogRead(freqPotPin), 0, 1024, 10000, 40000);
  analogWriteFrequency(motorPinPWM, writeFreq);

  float speed = map(analogRead(speedPotPin), 0, 1024, 0, 4096);
  analogWrite(motorPinPWM, speed);

  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);

  // every 50ms, print a value for the serial plotter
  if (timeMillis > 50) {
    encoderMotor.step();
    Serial.println(analogRead(speedPotPin));
    // Serial.print("writeFreq=");
    // Serial.print(writeFreq / 1000.0, DEC);
    // Serial.print(", speed=");
    // Serial.print(speed / 100, DEC);
    // Serial.print(", motorFreq=");
    // Serial.print(encoderMotor.frequency * 10.0, DEC);
    // Serial.println();
    timeMillis = 0;
  }

  //delay(1);
  delay(1.0);
}
