#include <Arduino.h>
#include "headers/uart.h"

#include <Wire.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>

#define TXD_PIN 17
#define RXD_PIN 16

uint8_t val[4];
String bin_out;

String right_x, left_x, colour_bin, colour;

String object_coord;

float screen_centre = 320;

int current_rover_x = 0;
int current_rover_y = 0;

float left_x = 120;
float right_x = 200;


const char* hex_char_to_bin(char c){
    // TODO handle default / error
    switch(toupper(c))
    {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
    }
}

const String  bin_to_colour (String c){
   
    String colour;

    if (c == "001" ){
        colour = "red";
    }
    else if(c == "010"){
        colour = "green";
    }
    else if (c == "011"){
        colour = "blue";
    }
    else if (c == "100"){
        colour = "orange";
    }
    else if (c == "101"){
        colour = "teal";
    }
    else if (c == "110"){
        colour = "fuchsia";
    }
    else if (c == "111"){
        colour = "fuchsia";
    }
    else {
        colour = "NULL";
    }
    
    return colour;
}

String hex_str_to_bin_str(const String& hex){
    // TODO use a loop from <algorithm> or smth

    String bin;

    for(unsigned i = 0; i != hex.length(); ++i){
       bin += hex_char_to_bin(hex[i]);
    }

    return bin;
}

int bin_str_to_dec_num(const String& binaryString){

  int value = 0; 
	int indexCounter = 0; 
	for(int i = binaryString.length()-1; i >= 0; i--) { 
 
      if(binaryString [i] == '1') { 
        value += pow(2, indexCounter); 
    	} 

        indexCounter++; 
	} 
	return value;
}

float size_to_sf(float left_x, float right_x){
  float sf = 0;
  size = right_x - left_x;

  sf = 80/size;

  return sf;

}

float find_angle(float left_x, float right_x){                                   
    float angle = 0;                                                                                                                             
    float size = 0;    
    float box_centre = 0; 
    float size = 0;     
    int sf = size_to_sf(left_x, right_x);

    box_centre = (left_x + right_x)/2;
    dist = box_centre - screen_centre;
   
    size = right_x - left_x;                                 
    
    angle = sf * ((0.0738 * dist) - 23.2);
  
    return angle; // angle will be negative for left angle 
};

float find_horizontal_distance(float left_x, float right_x){
    float dist = 0;
    float box_centre = 0;
 
    box_centre = (left_x + right_x)/2;
    dist = box_centre - screen_centre;  //left is negative 

    return dist;

}; 

float find_vertical_distance(float left_x, float right_x){
  float x_dist = find_horizontal_distance(left_x, right_x);
  float y_dist = 0;
  float angle = find_angle(left_x, right_x); 

  y_dist = x_dist * tan(angle) * 57.2958; // converts from radians to degrees

  return y_dist;

}
                                                                                     
String find_coord(float left_x, float right_x, int current_rover_x, int current_rover_y){
    String coord= ""; 
    float object_x;
    float object_y;
    
    float y_dist = find_vertical_distance(left_x, right_x);
    float x_dist = find_horizontal_distance(left_x, right_x);
    
    object_x = current_rover_x + x_dist;
    object_y = current_rover_y  + y_dist;

    coord = String(object_x) + "," + String(object_y);

    return coord;

};

void setup(){

  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD_PIN, TXD_PIN);

}

void loop(){
  //add in if statemnt - if boudning box deafault or too small - ignore

 /* if (Serial2.available()> 0 ){
        
        Serial2.readBytes(val, 4);
        char hex_out[100];
        sprintf(hex_out, "%02x%02x%02x%02x", val[3], val[2], val[1], val[0]);
        
        Serial.print(hex_out);
        Serial.print(" ");
        bin_out = hex_str_to_bin_str(hex_out);
        Serial.println(bin_out);

        colour_bin = bin_out.substring(2,5);
        left_x = bin_str_to_dec_num(bin_out.substring(5,16));
        right_x = bin_str_to_dec_num(bin_out.substring(21,32));

        colour = bin_to_colour(colour_bin);

        Serial.print("colour_bin: "); Serial.println(colour_bin);
        Serial.print("colour: "); Serial.println(colour);
        Serial.print("left_x: "); Serial.println(left_x);
        Serial.print("right_x: "); Serial.println(right_x);
        delay(1000);
*/
        object_coord = find_coord(left_x.toFloat(), right_x.toFloat(), current_rover_x, current_rover_y);

        Serial.print("object_coord");
        Serial.println(object_coord);
    
   //}


}