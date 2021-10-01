#include <EEPROM.h>

float pidFake[3];

void setup() {
  Serial.begin(115200);
  Serial.println("right away");
  delay(1500);
  Serial.println("after 1500");
  // pidFake[0] = 5;
  // pidFake[1] = 6;
  // pidFake[2] = 7;
  // EEPROM.put(0, pidFake);
  EEPROM.get(0, pidFake);
  //Serial.println("test TEST TEST");
}

void loop(){
  for(int i = 0; i < 3; i++){
    Serial.println(pidFake[i]);
  }

  delay(1000);
}
