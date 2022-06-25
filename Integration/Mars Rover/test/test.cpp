#include <Arduino.h>
#include "SPI.h"
#include <Robojax_L298N_DC_motor.h>
#include <ADNS3080.h>
#include <iostream>
#include "motor_control.h"
#include <MPU6050_light.h>
#include "Wire.h"

unsigned long timerGyro = 0;
unsigned long timer = 0;

MPU6050 mpu(Wire);
float angle, relativeAngle, rampUp, rampDown;
bool rotateComplete;    

float total_x = 0;
float total_y = 0;
float total = 0;
int total_dy = 0;
int average_dy = 0;

int distanceToGo  = 150;
int Tp = 15; //Target power - percentage of max power of motor (power is also known as 'duty cycle' ) 
float Kp = 3; //the Constant 'K' for the 'p' proportional controller
float integral = 0; //initialize
float Ki = 0.025; //the Constant 'K' for the 'i' integral term
float derivative = 0; //initialize
float error = 0;
float lastError = 0; //initialize
int Kd = 3; //the Constant 'K' for the 'd' derivative term
int correction = 0;
int powerLeft = 0;
int powerRight = 0;

float total_x1 = 0;
float total_y1 = 0;
float total_y_1 = 0;

int x=0;
int y=0;

float distance_x=0;
float distance_y=0;

volatile byte movementflag=0;
volatile int xydat[2];

int tdistance = 0;

//functions before void setup

void driveStraight(void *pvParameters ) {
  Serial.print("im driving straight on core: ");
  Serial.println(xPortGetCoreID());

  int dist = (int)pvParameters;
  dist = 30;

  for(;;){
  Serial.print("in for loop ");

  while(total_y < dist) { 
  Serial.print("in while loop ");

    if ((millis() - timer >= 100) && Tp < 40) 
    {
    timer = millis();  //get ready for the next iteration
    Tp += 5;
    }

    int val = mousecam_read_reg(ADNS3080_PIXEL_SUM);
    MD md;
    mousecam_read_motion(&md);
    //Serial.print("\n\tS: ");
    //Serial.println(md.squal/4);

    distance_x = convTwosComp(md.dx);
    distance_y = convTwosComp(md.dy);


    total_x1 = total_x1 + distance_x;
    total_y1 = total_y1 + distance_y;
    md.y = md.y + md.dy;
    total_x = total_x1/44.2;
    total_y = total_y1/44.2;

    //mpu.update();
    error = mpu.getAngleZ(); // proportional
    if (error == 0) {
        integral = 0;
    }
   else {
       integral = integral + error ;
    }
    derivative = error - lastError; 
    correction = (Kp*(error) + Ki*(integral) + Kd*derivative) * -1;
    powerLeft = Tp - correction;
    powerRight = Tp + correction;  
    
    if (powerLeft > 45) {
      powerLeft = 45;
    }
    else if (powerRight > 45) {
      powerRight = 45;
    }

    robot.rotate(right_motor, powerRight, CW);
    robot.rotate(left_motor, powerLeft, CW);

    lastError = error;
    //Serial.println("Dist " + String(total_y) + "; error " + String(error) + "; power_left " + String(powerLeft) + "; powerRight " + String(powerRight));
    // Serial.println("\ttotal_y: "+String(total_y));
    Serial.println(error);
    delay(5);
  }

  
  robot.rotate(right_motor, 0, CW);
  robot.rotate(left_motor, 0, CW);

  }}


void Sensors(void *param){
 
  Serial.print("sensor update running on core");
  Serial.println(xPortGetCoreID());
  for(;;){
    mpu.update();
    delay(5);
   }

}

void Main(void *pvParameters){
  Serial.println("infinitely calling drive straight...");
  
  Serial.print("hello");
  delay(5);
  
}

TaskHandle_t sensorupdate;
TaskHandle_t testfunc;
TaskHandle_t straight;


void setup()
{    
  Serial.begin(115200);
  Wire.begin();

  delay(1000);
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(2000);
  mpu.calcOffsets(true, false); // gyro and accelero
  Serial.println("Done!\n");
  mpu.setFilterGyroCoef(1);

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

  if(mousecam_init()==-1)
  {
    Serial.println("Mouse cam failed to init");
    while(1);
  }

  xTaskCreatePinnedToCore(Main,"testfunc",10000,NULL,1,&testfunc,0); 
  delay(100); 
  xTaskCreatePinnedToCore(Sensors,"sensorupdate",10000,NULL,1,&sensorupdate,1);
  delay(100); 

byte frame[ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y];
}


void loop() {
}
  


