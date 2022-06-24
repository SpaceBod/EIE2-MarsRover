// TO DO 2: read correct values to database for mapping

#include "headers/control.h"

//_________________________________________________________________________DRIVE HEADERS________________________________________________________________________

#include "headers/motor_control.h"
#include "headers/ADNS3080.h"

// __________________________________________________________VARIABLE DELCARATION FOR CONTROL MODULE____________________________________________________________

const char* ssid = "AndroidAP";
const char* password = "33355777";

#define UDP_PORT 3003
WiFiUDP UDP;

char packet[255];
char reply[] = "Packet received!";

String prevPacket = "";
String curPacket = "";

int intArray[3];

String serverName = "http://192.168.0.51/get_rover_data.php";
const char* serverNameOne = "http://192.168.0.51/post_rover_data.php"; //wireless lan adapter wifi IPV4 address
 
String apiKey = "tPmAT5Ab3j7F9";

String rover_coord, fan_coord, alien_coord, alien_colour;
int rover_angle;

// __________________________________________________________VARIABLE DECLARATION FOR DRIVE MODULE_____________________________________________________________

int distance, degrees, power, power1, power2;
int direction, direction1, direction2;
int current_rover_x, current_rover_y;

unsigned long lastTime = 0;

// __________________________________________________________VARIABLE DECLARATION FOR RADAR MODULE_____________________________________________________________

#define RADAR_VOLTAGE 35 // pin A5
int radar_voltage = 0;

int radar_x = 0 ;
int radar_y = 0;

String radar_present = "";

//______________________________________________________________________________________________________________________________________________________________

void setup() {
// ___________________________________________________________________CONTROL MODULE____________________________________________________________________________
  
  Serial.begin(115200);
  initWiFi(ssid, password);

  UDP.begin(UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP_PORT);

// ___________________________________________________________________DRIVE MODULE______________________________________________________________________________
  
  //__________________________________________________________________MOTOR_CONTROL_____________________________________________________________________________
    
  int a = 0;
  robot.begin();

 //____________________________________________________________________OPTICAL_FLOW______________________________________________________________________________

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

//______________________________________________________________________________________________________________________________________________________________
   
void loop() {

//___________________________________________________________________UDP AND MANUAL CONTROL_____________________________________________________________________

  int packetSize = UDP.parsePacket();
  
  if (packetSize) {
    
    int len = UDP.read(packet, 255);
    
    if (len > 0) {
      
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

//___________________________________________________________________RADAR MODULE_____________________________________________________________________
  
  radar_voltage = analogRead(RADAR_VOLTAGE);
  Serial.println(radar_voltage);

  //TO DO: check if the radar values are being taken in continously whilst connected and tehrefore if the serial print is being ouputted with delay or if the values are being read with a delay

  if (radar_voltage>=2840){ //2.25V for 2840
    radar_present = "Yes";
    radar_x = current_rover_x;
    radar_y = current_rover_y;
    fan_coord = "(" + String(radar_x) + "," + String(radar_y) = ")";
  }

  else{
    radar_present = "No";
  }

  Serial.println ("radar? ");
  Serial.println(radar_present);

//___________________________________________________________________CONTROL MODULE_____________________________________________________________________
     
  unsigned long previousMillis = 0;
  unsigned long interval = 1000;
  unsigned long currentMillis = millis();
   
  //_______________________________________________________________RECONNECTING WIFI____________________________________________________________________
  
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

// ________________________________________________________________POSTING DATA_________________________________________________________________________

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
          
    //________________________________________________________________ASSIGNING RANDOM DATA______________________________________________________________
      
      rover_coord = "(" + String(generate_x()) + "," + String(generate_y()) = ")";
      rover_angle = generate_angle();

      alien_colour = generate_colour();
      alien_coord = "(" + String(generate_x()) + "," + String(generate_y()) = ")";
  
      Serial.print('\n');
      Serial.println("POST DATA");
        
      // Data to send with HTTP POST
      String httpRequestData = "api_key=" + apiKey + "&rover_coord=" + rover_coord + "&rover_angle=" + rover_angle + "&fan_coord=" + fan_coord + "&alien_coord=" + alien_coord + "&alien_colour=" + alien_colour + "";
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

 