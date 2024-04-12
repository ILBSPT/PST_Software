#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial Serial2(2,3); //RX, TX

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
}

void loop() {
  //while(Serial.available()) Serial2.write(Serial.read());
  while(Serial2.available()) Serial.print(Serial2.read(), HEX);
}