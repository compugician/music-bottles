#include "HX711.h"

#define calibration_factor 200
// Pins to the load cell amp
#define CLK 2     // clock pin to the load cell amp
#define DOUT 3    // data pin to the lca
HX711 scale(DOUT, CLK);

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
  
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();
}

void loop() {
  // send the value of analog input 0:
  Serial.println(scale.get_units());
  // wait a bit for the analog-to-digital converter
  // to stabilize after the last reading:
  delay(200);
}
