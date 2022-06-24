#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

const char* ssid = "LaptopWiFi";
const char* password = "summerProject1234";

//Your Domain name with URL path or IP address with path
String serverName = "http://146.169.196.225/get_rover_data.php";
const char* serverNameOne = "http://146.169.196.225/post_rover_data.php"; //wireless lan adapter wifi IPV4 address
 
String apiKey = "tPmAT5Ab3j7F9";

//Adafruit_BME280 bme;
int x_direction, y_direction, battery_life, angle;
String alien_present, infrastructure_present, colour, building_present;

String direction, degrees;


// ________________________________________________________control module _____________________________________________________________---

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("LaptopWiFi", "summerProject1234");
  Serial.print("Connecting to WiFi ..");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

//_________________________________________________GENERATING RANDOM DATA_____________________________________________________________________
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

  Serial.begin(115200);
  initWiFi();
  SPI.begin();

}

void loop() {

   unsigned long previousMillis = 0;
    unsigned long interval = 10000;
    unsigned long currentMillis = millis();
    
    //________________________________________RECONNECTING WIFI___________________________________________________________
   
    if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
      Serial.println(millis());
      Serial.println("Reconnecting to WiFi...");
     
      WiFi.begin("LaptopWiFi", "summerProject1234");

      while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
      }

      Serial.println(WiFi.localIP());
      
      previousMillis = currentMillis;
  }


    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
   
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverNameOne);
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
     // _____________________________________GENERATING RANDOM DATA_______________________________________________________
   
    x_direction = generate_x();
    y_direction = generate_y();
    angle = generate_angle();
    battery_life = generate_battery();
    alien_present = generate_alien();
    colour = generate_colour();
    infrastructure_present = generate_infrastructure();
    building_present = generate_building_present();

    //_______________________________________POSTING DATA_________________________________________________________________________________

      // Data to send with HTTP POST
        String httpRequestData = "api_key=" + apiKey + "&x_direction=" + x_direction + "&y_direction=" + y_direction + "&angle=" + angle + "&battery_life=" + battery_life + "&alien_present=" + alien_present + "&colour=" + colour + "&infrastructure_present=" +infrastructure_present + + "&building_present=" + building_present + "";
        Serial.println("httpRequestData: ");
        Serial.println(httpRequestData);
     
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      
      if (httpResponseCode>0) {
        Serial.println("HTTP Response code: ");
        Serial.println(httpResponseCode);
      }

     else {
      Serial.println("Error code: ");
      Serial.println(httpResponseCode);
    }
       // Free resources
       http.end();
    }

    //delay(30000); //posts every 30 seconds 
    delay(10000);

  //________________________________________________GETTING DATA_________________________________________________________________________
  

  // the following variables are unsigned longs because the time, measured in
  // milliseconds, will quickly become a bigger number than can be stored in an int.
  unsigned long lastTime = 0;

  // Set timer to 5 seconds (5000)
  unsigned long timerDelay = 5000;

  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.println("HTTP Response code: ");
        Serial.println(httpResponseCode);
        
        String payload = http.getString();
        Serial.println("Payload: ");
        Serial.println(payload);
      }

      //parse the payload into the degrees and direction 

      else {
        Serial.println("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}