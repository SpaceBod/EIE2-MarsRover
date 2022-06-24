#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include <Arduino_JSON.h> // DOWNLOAD LIBRARY?

#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5
#define RST_PIN 4
#define SS_PIN 2

const char* serverName = "http://192.168.0.8/post_rover_data.php"; //wireless lan adapter wifi IPV4 address
const char* serverNameGet = "http://192.168.0.8/get_rover_data.php"; 
String apiKey = "tPmAT5Ab3j7F9";

#define SEALEVELPRESSURE_HPA (1013.25)
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;
int x_direction, y_direction, battery_life;
String alien_present, infrastructure_present;

String roverReadings;
float roverReadingsArr[3];

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


String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
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

//_______________________________________________________________________________________________________________________________________________________

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
      http.begin(client, serverName);
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
     // _____________________________________GENERATING RANDOM DATA_______________________________________________________
   
    x_direction = generate_x();
    y_direction = generate_y();
    battery_life = generate_battery();
    alien_present = generate_alien();
    infrastructure_present = generate_infrastructure();

    //_______________________________________POSTING DATA_________________________________________________________________________________

      // Data to send with HTTP POST
        String httpRequestData = "api_key=" + apiKey + "&x_direction=" + x_direction + "&y_direction=" + y_direction + "&battery_life=" + battery_life + "&alien_present=" + alien_present + "&infrastructure_present=" +infrastructure_present + "";
        // String httpRequestData = "api_key=12345678&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";
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
    //________________________________________GETTING DATA____________________________________________________________________________________
      roverReadings = httpGETRequest(serverNameGet);
      Serial.println(roverReadings);
      JSONVar myObject = JSON.parse(roverReadings);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(myObject);
    
      // myObject.keys() can be used to get an array of all the keys in the object
      JSONVar keys = myObject.keys();
    
      for (int i = 0; i < keys.length(); i++) {
        JSONVar value = myObject[keys[i]];
        Serial.print(keys[i]);
        Serial.print(" = ");
        Serial.println(value);
        roverReadingsArr[i] = double(value);
      }
      Serial.print("1 = ");
      Serial.println(roverReadingsArr[0]);
      Serial.print("2 = ");
      Serial.println(roverReadingsArr[1]);
      Serial.print("3 = ");
      Serial.println(roverReadingsArr[2]);
    }

    // Free resources
    http.end();
    //delay(30000); //posts every 30 seconds 
    delay(10000);
 }
 

