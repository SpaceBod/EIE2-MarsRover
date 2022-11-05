#include <Arduino.h>
#include "SPI.h"

#include "headers/ADNS3080.h"

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

byte frame[ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y];

void setup()
{
  //L298N DC Motor by Robojax.com
  pinMode(PIN_SS,OUTPUT);
  pinMode(PIN_MISO,INPUT);
  pinMode(PIN_MOSI,OUTPUT);
  pinMode(PIN_SCK,OUTPUT);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);

  Serial.begin(115200);

  if(mousecam_init()==-1)
  {
    Serial.println("Mouse cam failed to init");
    while(1);
  }
}


void loop()
{


    if(movementflag){

    tdistance = tdistance + convTwosComp(xydat[0]);
    Serial.println("Distance = " + String(tdistance));
    movementflag=0;
    delay(1);
    }

  
  // if enabled this section grabs frames and outputs them as ascii art
/*
  if(mousecam_frame_capture(frame)==0)
  {
    int i,j,k;
    for(i=0, k=0; i<ADNS3080_PIXELS_Y; i++)
    {
      for(j=0; j<ADNS3080_PIXELS_X; j++, k++)
      {
        Serial.print(asciiart(frame[k]));
        Serial.print(' ');
      }
      Serial.println();
    }
  }
  */
  Serial.println();
  delay(1);


  // if enabled this section produces a bar graph of the surface quality that can be used to focus the camera
  // also drawn is the average pixel value 0-63 and the shutter speed and the motion dx,dy.

  int val = mousecam_read_reg(ADNS3080_PIXEL_SUM);
  MD md;
  mousecam_read_motion(&md);
  for(int i=0; i<md.squal/4; i++)
    Serial.print('*');
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
  total = total + 0.2*(float)md.dx;
  total_x = 0.2*total_x1;
  total_y = 0.2*total_y1;


  total_x = total_x1/157;
  total_y = total_y1/157;


  Serial.print('\n');


  Serial.println("Distance_x = " + String((float)((total_x))));
  Serial.println("Distance_y = " + String((float)((total_y))));
  Serial.print('\n');

  float real_x = (float)(total_x);
  delay(250);

}
