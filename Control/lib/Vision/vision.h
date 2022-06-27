#include <Arduino.h>
#include "headers/uart.h"

#include <Wire.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>

#define TXD_PIN 17
#define RXD_PIN 16

const char* hex_char_to_bin(char c);
const String  bin_to_colour (String c);
String hex_str_to_bin_str(const String& hex);
int bin_str_to_dec_num(const String& binaryString);
float size_to_sf(float left_x, float right_x);
float find_angle(float left_x, float right_x);
float find_horizontal_distance(float left_x, float right_x);
float find_vertical_distance(float left_x, float right_x);
String find_coord(float left_x, float right_x, int current_rover_x, int current_rover_y);
float find_building_size(float left_x, float right_x);