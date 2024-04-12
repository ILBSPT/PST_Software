#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
}

void loop() {
  while(Serial2.available()) Serial.println(Serial2.read(), HEX);
}