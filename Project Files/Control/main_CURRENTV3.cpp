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

// TO DO 2: read correct values to database for mapping

//_______________________________________________HEADERS FOR DRIVE MODULE_____________________________________________

#include "headers/motor_control.h"
#include "headers/ADNS3080.h"

//_______________________________________________VARIABLE DELCARATION FOR CONTROL MODULE___________________________________________________________________________

const char* ssid = "LaptopWiFi";
const char* password = "summerProject1234";

#define UDP_PORT 3003
WiFiUDP UDP;
char packet[255];
char reply[] = "Packet received!";
String prevPacket = "";
String curPacket = "";
int intArray[3];

//Your Domain name with URL path or IP address with path
String serverName = "http://146.169.202.101/get_rover_data.php";
const char* serverNameOne = "http://146.169.202.101/post_rover_data.php"; //wireless lan adapter wifi IPV4 address
 
String apiKey = "tPmAT5Ab3j7F9";

int x_direction, y_direction, battery_life, angle;
String alien_present, infrastructure_present, colour, building_present;

//_______________________________________________________VARIABLE DECLARATION FOR DRIVE MODULE_________________________________________________________________________________________

int distance, degrees, power, power1, power2;
int direction, direction1, direction2;

unsigned long lastTime = 0;

//________________________________________________________VARIABLE DECLARATION FOR RADAR MODULE_____________________________________________________

#define RADAR_VOLTAGE 35 //A5
int radar_voltage = 0;
String radar_present = "";

//____________________________________________VARIABLE DELCARATION FOR DRIVE MODULE______________________________________________________________
//_____________________________________________OPTICAL_FLOW____________________________________________________

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
float r_theta = 0;


// ________________________________________________________CONTROL MODULE _________________________________________________________

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

//_________________________________________________GENERATING RANDOM DATA____________________________________________________________
int generate_x(){
  int x;
  x = random(0,500);
  return x;
}

int generate_y(){
  int x;
  x = random(0,500);
  return x;
}

int generate_angle(){
  int x;
  x = random(0,360);
  return x;
}

int generate_battery(){
  int x;
  x = random(0,100);
  return x;
}

String generate_alien(){
  int x;
  String alien;
  x = random(0,100);
  if (x<50){
    alien = "yes";
  }
  else{
    alien = "no";
  }
  return alien;
}

String generate_colour(){
  int x;
  String alien;
  x = random(0,100);
  if (x<25){
    alien = "red";
  }
  else if (26<x && x<50){
    alien = "yellow";
  }

  else if (51<x && x<75){
    alien = "green";
  }

  else{
    alien = "blue";
  }
  return alien;
}

String generate_infrastructure(){
  int x;
  String alien;
  x = random(0,100);
  if (x<50){
    alien = "yes";
  }
  else{
    alien = "no";
  }
  return alien;
}

String generate_building_present(){
  int x;
  String alien;
  x = random(0,100);
  if (x<50){
    alien = "yes";
  }
  else{
    alien = "no";
  }
  return alien;
}

void setup() {
//_____________________________________________CONTROL MODULE_______________________________________________________
  
  Serial.begin(115200);
  initWiFi();

  UDP.begin(UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP_PORT);
  //SPI.begin();

//_____________________________________________DRIVE_MODULE_______________________________________________________
    
  //_________________________________________MOTOR_CONTROL______________________________________________________
    
  int a = 0;
  robot.begin();

 //_______________________________________OPTICAL_FLOW_____________________________________________________________

  //L298N DC Motor by Robojax.com
  pinMode(PIN_SS,OUTPUT);
  pinMode(PIN_MISO,INPUT);
  pinMode(PIN_MOSI,OUTPUT);
  pinMode(PIN_SCK,OUTPUT);

  SPI.begin();

  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
}
   
void loop() {
  int packetSize = UDP.parsePacket();
  if (packetSize) {
    
    int len = UDP.read(packet, 255);
    
    if (len > 0){
      packet[len] = '\0';
    }
    curPacket = String(packet);
    
    // prints new movement if different from previous
    if(curPacket != prevPacket) {
      prevPacket = curPacket;
      int ipos = 0;
      char *tok = strtok(packet, " ");

      while (tok) {
      
        if (ipos < 3) {
          intArray[ipos++] = atoi(tok);
        }

        tok = strtok(NULL, " ");
      }

      Serial.print("\nDist: ");
      Serial.print(intArray[0]);
      Serial.print(" Ang: ");
      Serial.print(intArray[1]);
      Serial.print(" Pow: ");
      Serial.println(intArray[2]);
      
      distance = intArray[0];
      degrees =  intArray[1];
      power = intArray[2];
    }

    else {
      // prints out movement being held
      Serial.print("->");
    }

    // movement functions
    // turn off motor
    if (curPacket == "0 0 0"){
      robot.brake(1);
      robot.brake(2);
    }
    
    // movements!
    else {

      // forwards and backwards (no angles)
      if (degrees == 0) {
        if (distance > 0) {
          direction = 1;
        }
        else {
          direction = 2;
        }
        robot.rotate(right_motor, power, direction);
        robot.rotate(left_motor, power, direction);
      }

      // rotation (no f/b)
      if (degrees != 0 && distance == 0) {
        if (degrees > 0) {
          direction1 = 2;
          direction2 = 1;
          power1 = power * 0.5;
        }
        else {
          direction1 = 1;
          direction2 = 2;
          power2 = power * 0.5;
        }
        robot.rotate(right_motor, power1, direction1);
        robot.rotate(left_motor, power2, direction2);
      }

      // rotation and f/b
      if (degrees != 0 && distance != 0) {
        if (degrees > 0 && distance > 0) {
          direction1 = 1;
          direction2 = 1;
          power1 = power * 0.5;
          power2 = power;
        }

        if (degrees < 0 && distance > 0) {
          direction1 = 1;
          direction2 = 1;
          power1 = power;
          power2 = power * 0.5;
        }

        if (degrees > 0 && distance < 0) {
          direction1 = 2;
          direction2 = 2;
          power1 = power * 0.5;
          power2 = power;
        }

        if (degrees < 0 && distance < 0) {
          direction1 = 2;
          direction2 = 2;
          power1 = power;
          power2 = power * 0.5;
        }

        robot.rotate(right_motor, power1, direction1);
        robot.rotate(left_motor, power2, direction2);
      }
        
      
    }  
  }

//____________________________________________RADAR MODULE___________________________________________________________________________
  
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

//____________________________________________CONTROL_MODULE__________________________________________________________________________
   
  unsigned long previousMillis = 0;
  unsigned long interval = 1000;
  unsigned long currentMillis = millis();
   
 //________________________________________RECONNECTING WIFI___________________________________________________________
  
 if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
     Serial.println(millis());
     Serial.println("Reconnecting to WiFi...");
    
     WiFi.begin(ssid, password);

     while (WiFi.status() != WL_CONNECTED) {
       Serial.print('.');
       delay(1000);
     }

     Serial.println(WiFi.localIP());
     
     previousMillis = currentMillis;
 }

// _______________________________________POSTING DATA____________________________________________________________________

// unsigned long timerInterval = 10000;
// unsigned long startTime = millis();

//Send an HTTP POST request every 1 sec

  if ((millis() - lastTime) > 1000UL) { 
    
    lastTime = millis();

    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverNameOne);
        
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          
//_____________________________________ ASSIGNING RANDOM DATA_______________________________________________________
      
      x_direction = generate_x(); 
      y_direction = generate_y();
      angle = generate_angle(); 
      battery_life = generate_battery();
      alien_present = generate_alien();
      colour = generate_colour();
      infrastructure_present = radar_present;
      building_present = generate_building_present();

      Serial.print('\n');
      Serial.println("POST DATA");
        
      // Data to send with HTTP POST
      String httpRequestData = "api_key=" + apiKey + "&x_direction=" + x_direction + "&y_direction=" + y_direction + "&angle=" + angle + "&battery_life=" + battery_life + "&alien_present=" + alien_present + "&colour=" + colour + "&infrastructure_present=" +infrastructure_present + + "&building_present=" + building_present + "";
      Serial.print("httpRequestData: ");
      Serial.println(httpRequestData);
          
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
          
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
      }

      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
          
      // Free resources
    http.end();

    }

  }

  
}

 