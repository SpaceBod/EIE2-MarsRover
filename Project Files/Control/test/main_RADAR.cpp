#include <Arduino.h>
#include "SPI.h"
#include <iostream>

#define RADAR_VOLTAGE 35 //A5

float radar_voltage = 0; 
String radar_present = " ";

void setup() {
  Serial.begin(115200);
  delay(1000); // 1 second
}

void loop() {
  // Reading VOLTAGE from RADAR module
  radar_voltage = analogRead(RADAR_VOLTAGE);
  Serial.println(radar_voltage);

  if (radar_voltage>=2840){ //2.25V for 2840
    radar_present = "Yes";
  }

  else{
    radar_present = "No";
  }

  Serial.println ("radar? ");
  Serial.println(radar_present);

  delay(5000);

}