#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>

void initWiFi(const char* ssid, const char* password);
int generate_x();
int generate_y();
int generate_angle();
int generate_battery();
String generate_alien();
String generate_colour();
String generate_infrastructure();
String generate_building_present();
