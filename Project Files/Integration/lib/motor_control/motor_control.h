#pragma once 
#include <Arduino.h>
#include "SPI.h"
#include <iostream>

#include <Robojax_L298N_DC_motor.h>

// right settings
#define CHA 0

#define ENA 27 // this pin must be PWM enabled pin if Arduino board is used

#define IN1 26

#define IN2 25

// left settings

#define IN3 33

#define IN4 32

#define ENB 16 // this pin must be PWM enabled pin if Arduino board is used

#define CHB 1

const int CCW = 2; // do not change

const int CW  = 1; // do not change

#define right_motor 1 // do not change

#define left_motor 2 // do not change

// for two motors without debug information // Watch video instruciton for this line: https://youtu.be/2JTMqURJTwg

Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB);
// for two motors with debug information
//Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA, IN3, IN4, ENB, CHB, true);
