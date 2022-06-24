#include "headers/motor_control.h"
#include "headers/ADNS3080.h"

#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>

float total_x = 0;
float total_y = 0;
float total = 0;


float total_x1 = 0;
float total_y1 = 0;


int x=0;
int y=0;

int a=0;
int b=0;

float distance_x=0;
float distance_y=0;

volatile byte movementflag=0;
volatile int xydat[2];

int tdistance = 0;

int counter =0;
float total_dy= 0;
float average_dy = 0;


byte frame[ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y];
float m_dtheta = 0;
float m_dtheta_total = 0;
float m_theta = 0;
float r_theta = 180;

void setup()
{  

  Serial.begin(115200);
  robot.begin();
  //L298N DC Motor by Robojax.com
  pinMode(PIN_SS,OUTPUT);
  pinMode(PIN_MISO,INPUT);
  pinMode(PIN_MOSI,OUTPUT);
  pinMode(PIN_SCK,OUTPUT);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);

  if(mousecam_init()==-1) {
    Serial.println("Mouse cam failed to init");
    while(1);
  }
}


void loop(){

  int val = mousecam_read_reg(ADNS3080_PIXEL_SUM);
  MD md;
 
  //Checks resolution of the camera 
  mousecam_read_motion(&md);
  for(int i=0; i<md.squal/4; i++){
    Serial.print('*');
  }
  Serial.print(' ');
  Serial.print((val*100)/351);
  Serial.print(' ');
  Serial.print(md.shutter); Serial.print(" (");
  Serial.print((int)md.dx); Serial.print(',');
  Serial.print((int)md.dy); Serial.println(')');

  // Serial.println(md.max_pix);
  delay(1);

  distance_x = convTwosComp(md.dx);
  distance_y = convTwosComp(md.dy);

  total_x1 = total_x1 + distance_x;
  total_y1 = total_y1 + distance_y;
  md.x = md.x + 0.2*(float)md.dx;
  total_x = 0.2*total_x1;
  total_y = 0.2*total_y1;

  m_dtheta = (((0.2*md.dx)/125)/3.141592653)*180;
  m_dtheta_total = m_dtheta_total + m_dtheta;
  m_theta = (((md.x/125)/3.141592653)*180);

  Serial.println("md.x = " + String((int)((md.x))));

  Serial.println("m_theta = " + String((float)((m_theta))));

  
  float real_x = (float)(total_x);
  Serial.println("m_dtheta = " + String((float)((m_dtheta))));
  Serial.println("m_theta = " + String((float)((m_theta))));

  float speed = 0; 

 /* if (r_theta > 0) {
    while (r_theta > m_theta){

        speed = (r_theta - m_theta)*(m_theta/50);
        //Serial.print ("Speed: "); Serial.println(speed);
        Serial.print ("m_theta: "); Serial.println(m_theta);

        robot.rotate (right_motor, speed, CCW);
        robot.rotate(left_motor, speed, CW);       

   
    }*/
  
  robot.brake(1);
  robot.brake(2);

  }

