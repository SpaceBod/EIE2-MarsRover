#include "control.h"
//_________________________________________________________________________DRIVE HEADERS________________________________________________________________________

#include "motor_control.h"
#include "ADNS3080.h"
#include "drive.h"

// __________________________________________________________VARIABLE DELCARATION FOR CONTROL MODULE____________________________________________________________

const char* ssid = "roverspot";
const char* password = "roverspot";

const char* serverName = "http://146.169.196.222/post_rover_data.php"; //wireless lan adapter wifi IPV4 address
 
String apiKey = "tPmAT5Ab3j7F9";

String rover_coord, fan_coord, alien_coord, alien_colour, building_coord;
int rover_angle, battery_life;
float building_size;

char packet[255];
char reply[] = "Packet received!";

String prevPacket = "";
String curPacket = "";

#define UDP_PORT 3003
WiFiUDP UDP;

int UDPdistance = 0;
int UDPdegrees = 0;
int UDPpower = 0;

int intArray[3];

bool remote = false;

// __________________________________________________________VARIABLE DECLARATION FOR DRIVE MODULE_____________________________________________________________

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

//_______________________________________________________________FUNCTIONS____________________________________________________________________________________

void coords(){ //currx and curry are global - access directly when sending to sql 

    currangle = totalangle;
    //mdmancoord declared at top with other coord vars;
    //mousecam_read_motion(&mdmancoord);
     
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

void checkRadar() {
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
}

void reconnectWIFI() {
  unsigned long previousMillis = 0;
  unsigned long interval = 1000;
  unsigned long currentMillis = millis();
   
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
}

void postData() {
  if ((millis() - lastTime) > 1000UL) { //every 1 second
    
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

      battery_life = 0;//generate_battery_life();

      building_size = 0;//generate_battery_life();
  
      Serial.print('\n');
      Serial.println("POST DATA");
        
      // Data to send with HTTP POST
      String httpRequestData = "api_key=" + apiKey + "&rover_coord=" + rover_coord + "&rover_angle=" + rover_angle + "&fan_coord=" + fan_coord 
                             + "&alien_coord=" + alien_coord + "&alien_colour=" + alien_colour + "&building_coord" + building_coord + "&building_size" 
                             + building_size + "&battery_life" + battery_life + "";
     
      //Serial.print("httpRequestData: ");
      //Serial.println(httpRequestData);
          
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
          
      if (httpResponseCode>0) {
      //  Serial.print("HTTP Response code: ");
       // Serial.println(httpResponseCode);
      }

      else {
      //  Serial.print("Error code: ");
       // Serial.println(httpResponseCode);
      }
          
      // Free resources
    http.end();

    }
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

void UDP_listen(){
  //Serial.println("LISTENING UDP");
  int packetSize = UDP.parsePacket();

  if (packetSize) {  
    
    int len = UDP.read(packet, 255);
    
    if (len > 0) {
      
      packet[len] = '\0';

    }
    curPacket = String(packet);
    //Serial.println(curPacket);
    if(curPacket == "true"){
      Serial.println("remote set to true");
      remote = true;
    }
    if(curPacket == "false"){
      Serial.println("remote set to false");
      remote = false;
    }
  
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

      /*Serial.print("\nDist: ");
      Serial.print(intArray[0]);
      Serial.print(" Ang: ");
      Serial.print(intArray[1]);
      Serial.print(" Pow: ");
      Serial.println(intArray[2]);*/
      
      UDPdistance = intArray[0];
      UDPdegrees =  intArray[1];
      UDPpower = intArray[2];

    }

    else {
      // prints out movement being held
     // Serial.print("->");
    }
    
  }
  else {
    //remote = false;
  }
}  

void Sensors(void *param){
 
  Serial.println("Sensor updates running on core: " + String(xPortGetCoreID()));
  for(;;){ //infinitely run this loop for updating gyro & ultrasonic info
    
    mpu.update();
    if (recenter == true) {
      //Serial.println("recentered");
      initial_angle = mpu.getAngleZ();
      relative_angle = mpu.getAngleZ() - initial_angle;
      gyro_angle = abs(relative_angle);
      recenter = false;
    }
    Ultrasonic();
    delay(5);

    UDP_listen();
    reconnectWIFI();
  //  postData();
    
   }

}

void rotate(float r_angle, bool manual){    

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

  if(manual == false){
    totalangle += r_angle;
  }

  while(gyro_angle < angleSign * r_angle && curPacket != "0 0 0") { 
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
  curPacket = "0 0 0";


  if(manual == true){
    totalangle += angleSign * gyro_angle;
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


void driveStraight(int dist, bool manual) {
int distsign;

  if(dist < 0){
     distsign = -1;
  }
  else{
     distsign = 1;
  }
  dist = abs(dist);

  recenter = true;
  delay(10);
  //Serial.println("in driveStraight running on core: " + String(xPortGetCoreID()));
  dist = dist + total_y;

  int maxpower = 40;
   if(manual == true){ //if remote control on, use power from remote.
        maxpower = UDPpower;
    }

  while(total_y < dist && frontsensor != true && curPacket != "0 0 0") { //still stops during manual - stops you crashing into a wall
    relative_angle = mpu.getAngleZ() - initial_angle;
    if ((millis() - timer >= 100) && Tp < maxpower) 
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

   
    
    if(powerLeft > maxpower + 5) {
      powerLeft = maxpower + 5;
    }
    else if (powerRight > maxpower + 5) {
      powerRight = maxpower + 5;
    }


    if(distsign == 1){
        robot.rotate(right_motor, powerRight, CW);
        robot.rotate(left_motor, powerLeft, CW);
    }
    else{ //if -ve dist from manual control
        robot.rotate(right_motor, powerLeft, CCW);
        robot.rotate(left_motor, powerRight, CCW);
    }

   


    lastError = error;
    //Serial.println("Dist " + String(total_y) + "; error " + String(error) + "; power_left " + String(powerLeft) + "; powerRight " + String(powerRight));
    // Serial.println("\ttotal_y: "+String(total_y));
    // Serial.println("Error: " + String(error));    
    coords();
    Serial.println(String(currx) + " , " + String(curry));


    delay(50);
  }
  curPacket = "0 0 0";


  robot.rotate(right_motor, 0, CW);
  robot.rotate(left_motor, 0, CW);

 if(manual == false){
    if (frontsensor == true && leftUSTriggered == false && rightUSTriggered == false) {
      find_perpendicular();
    }
    else if (frontsensor == true && leftUSTriggered == true && rightUSTriggered == false) {
        rotate(90, false); 
    }
    else if(frontsensor == true && leftUSTriggered == false && rightUSTriggered == true){ //not sure if minus angles work 
        rotate(-90, false);
    }

  }   

}

void manual_control(){
    //Serial.println("ENTERED MANUAL CONTROL");
    //Serial.println(curPacket);
    if(curPacket == "0 0 0" || (UDPdistance == 0 && UDPdegrees == 0 && UDPpower == 0)){
        robot.brake(1);
        robot.brake(2);
    }
    else{ //movements:
            //udppower applied in drivestraight function.
        if(UDPdegrees == 0){//just forward/back
            driveStraight(UDPdistance, true);//dont use avoidance
            delay(5);
        }

        else if(UDPdegrees != 0 && UDPdistance == 0){ //if just rotate
            if (UDPdegrees == abs(255)) {
              rotate(static_cast<float>(UDPdegrees), true);
            }
            else{
              rotate(static_cast<float>(UDPdegrees), false); 
            }
            
            delay(5);

        }

        else if(UDPdegrees != 0 && UDPdistance !=0){//forward/back and rotate
            //should work fine with slider, not sure about controller
            rotate(static_cast<float>(UDPdegrees), true); //do rotation 
            driveStraight(UDPdistance, true);  //then forward the distance
            //hopefully will allow coord tracking and not move in a stupid way. small increments so maybe works?
        }

    }
}

void auto_drive(){
  Serial.println("IN AUTO");
   driveStraight(10, false);

   delay(1000);
}

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
  
  while (remote == false){
    auto_drive();
  }
  while (remote == true){
    
    manual_control();

   // Serial.println("x: " + String(currx));
    //Serial.print(" y: " + String(curry));
  }

}



 
