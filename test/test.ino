#include <EEPROM.h>

float* pidFake = new float[3]

void setup() {
  Serial.begin(115200);
  pidFake[0] = 5;
  pidFake[1] = 6;
  pidFake[2] = 7;
  EEPROM.put(0, *pidFake);
  Serial.println("test TEST TEST");
}

void loop(){
  delay(10);
}
