#pragma once
#include <Arduino.h>
#include "SPI.h"
#include <Robojax_L298N_DC_motor.h>
#include <ADNS3080.h>
#include <iostream>
#include "motor_control.h"
#include <MPU6050_light.h>
#include "Wire.h"
#include  <math.h>

void coords();
void Ultrasonic();
void Sensors(void *param);
void rotate(float r_angle);
void perpTurn();
void find_perpendicular();
void driveStraight(int dist);
void UDP_listen();
void manual_control();
void auto_drive();