#include <arduino.h>

#include <max6675.h>


MAX6675 thermocouple(18, 0, 19);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);

  while (!Serial);
  while (!Serial2);

}

void loop() {
    // basic readout test, just print the current temp
  
   //Serial.print("C = "); 
   //Serial.println(thermocouple.readCelsius());
   //Serial.print("F = ");
   //Serial.println(thermocouple.readFahrenheit());
 
   // For the MAX6675 to update, you must delay AT LEAST 250ms between reads!
   //delay(1000);

  while(Serial2.available()) Serial.write(Serial2.read());
}