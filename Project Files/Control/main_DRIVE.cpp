#include <Arduino.h>
#include "SPI.h"
#include <Robojax_L298N_DC_motor.h>
#include <ADNS3080.h>
#include <iostream>
#include "motor_control.h"
#include <MPU6050_light.h>
#include "Wire.h"
#include  <math.h>

unsigned long timerGyro = 0;
unsigned long timer = 0;

MPU6050 mpu(Wire);
float angle;
bool rotateComplete;    

float total_x = 0;
float total_y = 0;
float target_y = 0;
float total = 0;
int total_dy = 0;
int average_dy = 0;
float gyro_angle = 0;
float initial_angle = 0;
float relative_angle = 0;
float rotationSpeed = 0;
bool in_range = true;
float angleSign = 0;
bool toggleAngle =false;

MD md;

bool recenter = false;

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

float prev_distance = 999;
float min_distance = 999;
float min_angle;

int x=0;
int y=0;

float distance_x=0;
float distance_y=0;

volatile byte movementflag=0;
volatile int xydat[2];

float currangle = 0;
float currx = 0;
float curry = 0;
MD mdmancoord;
float totalangle = 0;

int tdistance = 0;

//ultrasonic stuff:
#define echoPin 13 // Echo Pin Forward Sensor
#define trigPin 12 // Trigger Pin Forward Sensor
#define leftHigh 15 // Left Sensor IO
#define rightHigh 2 // Right  Right Sensor IO
bool leftUSTriggered = false;
bool rightUSTriggered = false;
double duration, distance; // Duration used to calculate distance
bool frontsensor = false;

void coords(){ //currx and curry are global - access directly when sending to sql

    currangle = totalangle;
    //mdmancoord declared at top with other coord vars;
    //mousecam_read_motion(&mdmancoord);
     
    float changex = convTwosComp(md.dx);
    float changey = convTwosComp(md.dy);

    changey = changey/44.2;

    while(currangle < 0){ //conv to +ve angles only
      currangle = currangle + 360;
    }

    while(currangle > 360){
      currangle = currangle - 360;
    }

    float relang = 0;
    float accy = 0;
    float accx = 0;

    if(currangle >= 0 && currangle <=90 ){
      relang = currangle;
      relang = (relang * 71)/4068;
      accy = cos(relang)*changey; //dist travelled in y direction w.r.t start
      curry = curry + accy;
      accx = sin(currangle)*changey;
      currx = currx + accx;

    }

    else if(currangle > 90 && currangle <= 180){
      relang = 180 - currangle;
      relang = (relang * 71)/4068;
      accy = cos(relang)*changey;
      curry = curry - accy;
      accx = sin(relang)*changey;
      currx = currx + accx;
    }
    else if(currangle > 180 && currangle <= 270){
      relang = currangle - 180;
      relang = (relang * 71)/4068;
      accy = cos(relang)*changey;
      curry = curry - accy;
      accx = sin(relang)*changey;
      currx = currx - accx;
    }
    else{
      relang = 360 - currangle;
      relang = (relang * 71)/4068;
      accy = cos(relang)*changey;
      curry = curry + accy;
      accx = sin(relang)*changey;
      currx = currx - accx;

    }
}


void Ultrasonic(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  //Calculate the distance (in cm) based on the speed of sound.
  float actdistance = duration/58.2;
  if (actdistance >= 4 && actdistance <= 15) {
    distance = actdistance;
    frontsensor = true;
  }
  else{
    frontsensor = false;
  }
  if (digitalRead(leftHigh) == HIGH && leftUSTriggered == false) {
    Serial.print("\nLeft Active ");
    leftUSTriggered = true;
  }
  else if (digitalRead(leftHigh) == LOW) {
    if (leftUSTriggered == true) {
      Serial.print("\nLeft Inactive");
    }
    leftUSTriggered = false;
  }
  if (digitalRead(rightHigh) == HIGH && rightUSTriggered == false) {
    Serial.print("\nRight Active ");
    rightUSTriggered = true;
  }
  else if (digitalRead(rightHigh) == LOW) {
    if (rightUSTriggered == true) {
      Serial.print("\nRight Inactive");
    }
    rightUSTriggered = false;
  }
}

void Sensors(void *param){
 
  Serial.println("Sensor updates running on core: " + String(xPortGetCoreID()));
  for(;;){ //infinitely run this loop for updating gyro & ultrasonic info
    mpu.update();
    if (recenter == true) {
      Serial.println("recentered");
      initial_angle = mpu.getAngleZ();
      relative_angle = mpu.getAngleZ() - initial_angle;
      gyro_angle = abs(relative_angle);
      recenter = false;
    }
    Ultrasonic();
    delay(5);
   }

}

TaskHandle_t sensorupdate;

void setup()
{ 
  Serial.begin(115200);
  Wire.begin();
  
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

  //ultrasonic:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(leftHigh, INPUT);
  pinMode(rightHigh, INPUT);

  xTaskCreatePinnedToCore(Sensors,"sensorupdate",10000,NULL,1,&sensorupdate,0); //set sensors on core 0 only
}

byte frame[ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y];

void rotate(float r_angle)
{   
  totalangle += r_angle;

  recenter = true;
  delay(10);
  if (r_angle == 90) {
    r_angle = 86.5;
  }
  else if (r_angle == -90) {
    r_angle = -86.5;
  }
  if (r_angle > 0) {
    angleSign = 1;
  }
  else {
    angleSign = -1;
  }

  while(gyro_angle < angleSign * r_angle) { 
    in_range = true;
    relative_angle = mpu.getAngleZ() - initial_angle;
    gyro_angle = abs(relative_angle);   
    // Serial.println(String(initial_angle) + " ::: " + String(relative_angle));

    // RAMP UP   
    if ((millis() - timer >= 100) && rotationSpeed < 35 && 10 > gyro_angle) 
    {
      timer = millis();  //get ready for the next iteration
      rotationSpeed += 3;
      Serial.println("Ramp Up");

      in_range = false;
    }

    // RAMP DOWN
    if ((millis() - timer >= 200) && rotationSpeed > 20 && r_angle - 20 < gyro_angle) 
    {
      timer = millis();
      rotationSpeed -= 4;
      in_range = false;
    }
    if (angleSign == 1) {
      robot.rotate(right_motor, rotationSpeed, CCW);
      robot.rotate(left_motor, rotationSpeed, CW);
    }

    else {
      robot.rotate(right_motor, rotationSpeed, CW);
      robot.rotate(left_motor, rotationSpeed, CCW);
      }
    
    // Serial.println(String(angleSign * gyro_angle));
    delay(5);
  }
  robot.rotate(right_motor, 0, CW);
  robot.rotate(left_motor, 0, CW);
}

void perpTurn() {   
  recenter = true;
  delay(10);

  while(gyro_angle < 135) { 
    relative_angle = mpu.getAngleZ() - initial_angle;
    gyro_angle = abs(relative_angle);
    angleSign = 1;   
    
    if (angleSign == 1) {
      robot.rotate(right_motor, 20, CCW);
      robot.rotate(left_motor, 20, CW);
      if (leftUSTriggered == true && gyro_angle > 20){
        recenter = true;
        delay(500);
        // while(gyro_angle < 1) { 
        //   relative_angle = mpu.getAngleZ() - initial_angle;
        //   gyro_angle = abs(relative_angle);
        //   robot.rotate(right_motor, 20, CCW);
        //   robot.rotate(left_motor, 20, CW);
        // }
        robot.rotate(right_motor, 0, CW);
        robot.rotate(left_motor, 0, CW);
        break;
      }
    }
    delay(5);
  }

  robot.rotate(right_motor, 0, CW);
  robot.rotate(left_motor, 0, CW);
}

void find_perpendicular() {
  perpTurn();
}

void driveStraight(int dist) {
  recenter = true;
  delay(10);
  Serial.println("driveStraight running on core: " + String(xPortGetCoreID()));
  dist = dist + total_y;

  while(total_y < dist && frontsensor != true) {
    relative_angle = mpu.getAngleZ() - initial_angle;
    if ((millis() - timer >= 100) && Tp < 40) 
    {
    timer = millis();  //get ready for the next iteration
    Tp += 5;
    }

    int val = mousecam_read_reg(ADNS3080_PIXEL_SUM);
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

    //mpu.update(); // dont need the update in here anymore - updates on sensor code core 0
    error = relative_angle; // proportional
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
    // Serial.println("Error: " + String(error));    
    coords();

    delay(50);
  }

  robot.rotate(right_motor, 0, CW);
  robot.rotate(left_motor, 0, CW);

  if (frontsensor == true && leftUSTriggered == false && rightUSTriggered == false) {
     find_perpendicular();
  }
  else if (frontsensor == true && leftUSTriggered == true && rightUSTriggered == false) {
     rotate(90); 
  }
  }

void loop() {
  delay(500);
  driveStraight(400);
  delay(500);
  driveStraight(20);
  rotate(90);

  // delay(1000);
  // driveStraight(50);
  // Serial.println("x: " + String(currx) + " y: " + String(curry));
  // delay(1000);
  // Serial.println("x: " + String(currx) + " y: " + String(curry));
  // delay(1000);
  // driveStraight(10);
  // Serial.println("x: " + String(currx) + " y: " + String(curry));
  // delay(1000);
  // Serial.println(distance);
  // delay(100);

  // delay(100);
}
  


