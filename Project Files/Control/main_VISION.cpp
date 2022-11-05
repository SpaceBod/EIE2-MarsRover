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

int current_rover_x = 0;
int current_rover_y = 0;

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

int size_to_dist(int size){
    int dist = 0; 
    //model of relationship 

    return dist; 
}

int find_distance(int left_x, int right_x){
    int dist = 0;
    int size = 0;

    // find size of bounding box
    size = right_x - left_x;

    // model the relationship between the distance of the object and size of box and use this relationship to find distance 
    dist = size_to_dist(size);
    return dist;

};                                                                                   //           dist_box_to_screen
                                                                                     //            O ____________________________
int find_angle(int left_x, int right_x, int dist){                                   //              \             |            /
    int angle = 0;                                                                   //        dist    \           |          /
    int box_centre = 0;                                                              //                  \         |        /  
    int screen_centre = 0;                                                           //                    \       |      /
    int dist_box_to_screen = 0;                                                      //                      \ ang |    /
                                                                                     //                        \   |  /
    //find centre of bounding box                                                                                 R
    box_centre = (right_x + left_x)/2;
    
    // use socahtoa to find the angle 
    dist_box_to_screen = box_centre - screen_centre; //negative values for things to left, postive for right an
    angle = asin(dist_box_to_screen/dist);

    return angle; // angle will be negative for left angle 

}; //change this to give a model for the angle on screen to angle of rover so we can avoid pythag

String find_coord(int left_x, int right_x, int current_rover_x, int current_rover_y){
    String coord= ""; 
    int object_x;
    int object_y;
    
    int dist_box_to_screen = 0;
    int box_centre = 0;  
    int screen_centre = 0; 

    int y_dist = 0;
    int dist = find_distance(left_x, right_x);
    
    box_centre = (right_x + left_x)/2;
    dist_box_to_screen = box_centre - screen_centre;
    object_x = current_rover_x + dist_box_to_screen;

    y_dist = sqrt(pow(dist,2) - pow(dist_box_to_screen,2)); //pythagoras
    object_y = current_rover_y  + y_dist;

    coord = String(object_x) + "," + String (object_y);

    return coord;

};

void setup(){

  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD_PIN, TXD_PIN);

}

void loop(){

  if (Serial2.available()> 0 ){
        
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

        object_coord = find_coord(left_x.toInt(), right_x.toInt(), current_rover_x, current_rover_y);
    
   }


}