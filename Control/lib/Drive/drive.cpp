#include "drive.h"

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

    UDP_listen();
    
   }

}

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

void UDP_listen(){
  int packetSize = UDP.parsePacket();

  if (packetSize) {

    remote = true
    
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
    
  }
  else {
    remote = false;
  }
}  

void manual_control(){
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

void auto_drive(){
   driveStraight(10);
   rotate(90);
   driveStraight(10);
   rotate(90); 
}