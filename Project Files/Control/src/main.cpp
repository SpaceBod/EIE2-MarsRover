// TO DO 2: read correct values to database for mapping

#include "control.h"

//_________________________________________________________________________DRIVE HEADERS________________________________________________________________________

#include "motor_control.h"
#include "ADNS3080.h"
#include "drive.h"

// __________________________________________________________VARIABLE DELCARATION FOR CONTROL MODULE____________________________________________________________

const char* ssid = "LaptopWiFi";
const char* password = "summerProject1234";

#define UDP_PORT 3003
WiFiUDP UDP;

char packet[255];
char reply[] = "Packet received!";

String prevPacket = "";
String curPacket = "";

int intArray[3];

const char* serverName = "http://146.169.202.58/post_rover_data.php"; //wireless lan adapter wifi IPV4 address
 
String apiKey = "tPmAT5Ab3j7F9";

String rover_coord, fan_coord, alien_coord, alien_colour, building_coord;
int rover_angle;

// __________________________________________________________VARIABLE DECLARATION FOR DRIVE MODULE_____________________________________________________________

int distance, degrees, power, power1, power2;
int direction, direction1, direction2;

bool remote = false;

unsigned long lastTime = 0;

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


// __________________________________________________________VARIABLE DECLARATION FOR RADAR MODULE_____________________________________________________________

#define RADAR_VOLTAGE 35 // pin A5
int radar_voltage = 0;

int radar_x = 0 ;
int radar_y = 0;

String radar_present = "";

//______________________________________________________________________________________________________________________________________________________________

TaskHandle_t sensorupdate;

void setup() {
// ___________________________________________________________________CONTROL MODULE____________________________________________________________________________
  
  Serial.begin(115200);
  initWiFi(ssid, password);

  UDP.begin(UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP_PORT);

// ___________________________________________________________________DRIVE MODULE______________________________________________________________________________
 
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

//______________________________________________________________________________________________________________________________________________________________

byte frame[ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y];

void loop() {

//___________________________________________________________________DRIVE MODULE_____________________________________________________________________
while (remote = false){
  auto_drive();
}

while (remote = true){
  manual_drive();
}

//___________________________________________________________________RADAR MODULE_____________________________________________________________________
  
  radar_voltage = analogRead(RADAR_VOLTAGE);
  Serial.println(radar_voltage);

  //TO DO: check if the radar values are being taken in continously whilst connected and tehrefore if the serial print is being ouputted with delay or if the values are being read with a delay

  if (radar_voltage >= 2840){ //2.25V for 2840
    radar_present = "Yes";
    radar_x = currx;
    radar_y = curry;
    fan_coord = String(radar_x) + "," + String(radar_y);
  }

  else{
    radar_present = "No";
   // fan_coord = "NULL";
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

  if ((millis() - lastTime) > 5000UL) { 
    
    lastTime = millis();

    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
        
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          
    //________________________________________________________________ASSIGNING RANDOM DATA______________________________________________________________
      
      rover_coord = String(currx) + "," + String(curry);
      rover_angle = currangle;

      alien_colour = generate_colour();
      alien_coord =  String(generate_x()) + "," + String(generate_y());

      building_coord = String(generate_x()) + "," + String(generate_y());
  
      Serial.print('\n');
      Serial.println("POST DATA");
        
      // Data to send with HTTP POST
      String httpRequestData = "api_key=" + apiKey + "&rover_coord=" + rover_coord + "&rover_angle=" + rover_angle + "&fan_coord=" + fan_coord + "&alien_coord=" + alien_coord + "&alien_colour=" + alien_colour + "&building_coord" + building_coord +"";
     
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

 