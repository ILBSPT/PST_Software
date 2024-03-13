#include <arduino.h>

float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void setup() {
  Serial.begin(9600);
  while (!Serial);

  //Serial.println("LoRa Receiver");

  /*
  LoRa.setPins(5,4,36);
  //LoRa.setSignalBandwidth(500E3);
  //LoRa.setCodingRate4(5);
  //LoRa.setSpreadingFactor(7);
  //LoRa.setGain(1);
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  uint8_t reg = LoRa.readRegister(0x1);
  Serial.println("Register");
  Serial.println(reg);

  */
}

void loop() {
  int analogValue = analogRead(39);
  float voltage = floatMap(analogValue, 0, 4095, 0, 3.3);

  Serial.print("Value ");
  Serial.println(voltage);
  delay(100);
  /*
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
  */


}