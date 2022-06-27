#include "vision.h"

float pixel_to_cm = 0.1390625;

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
        colour = "yellow";
    }
    else if (c == "101"){
        colour = "teal";
    }
    else if (c == "101"){
        colour = "fuchsia";
    }
    else if (c == "110"){
        colour = "fuchsia";
    }
    else if (c == "111"){
        colour = "building";
    }
    else {
        colour = "NULL";
    }
    
    return colour;
}

String hex_str_to_bin_str(const String& hex){
   
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
  float size = right_x - left_x;

  sf = 80/size;
//  Serial.println("sf " + String(sf)) ;
//  Serial.println("sf_Size " + String(size)) ;

  return sf;

}

float find_angle(float left_x, float right_x){                                   
    float angle = 0;                                                                                                                             
    float size = 0;    
    float box_centre = 0; 
   
    float sf = size_to_sf(left_x, right_x);

    box_centre = (left_x + right_x)/2;
    float dist = screen_centre - box_centre;
   
    size = right_x - left_x;                                 
    
    angle = sf * ((0.0738 * dist) - 23.2);
  //  Serial.println("find_angle angle " + String(angle)) ;

  
    return angle; // angle will be negative for left angle 
};

float find_horizontal_distance(float left_x, float right_x){
    float dist = 0;
    float box_centre = 0;
    float sf = size_to_sf(left_x,right_x);
     
    box_centre = (left_x + right_x)/2;
    dist = (box_centre - screen_centre); //left is negative. note the conversiob from pixel to cm - 1 pixel is 89/640cm
    float actualdist = dist * sf * pixel_to_cm;
    
//    Serial.println("horiz " + String(actualdist));

    return actualdist;

}; 

float find_vertical_distance(float left_x, float right_x){
  float x_dist = find_horizontal_distance(left_x, right_x);
  float y_dist = 0;
  float angle = find_angle(left_x, right_x); 
  float anglerads = (angle/180) * M_PI; 
//  Serial.println("rads " + String(anglerads));

  y_dist = x_dist / tan(anglerads); // converts from radians to degrees
 // Serial.println("vert " + String(y_dist)) ;

  return y_dist;

}
                                                                                     
String find_coord(float left_x, float right_x, int current_rover_x, int current_rover_y){
    //String coord= ""; 
    float object_x;
    float object_y;
    
    float y_dist = find_vertical_distance(left_x, right_x);
    float x_dist = find_horizontal_distance(left_x, right_x);
    
    object_x = current_rover_x + x_dist;
    object_y = current_rover_y  + y_dist;

    String coord = String(object_x, 2) +"," + String(object_y, 2);

    return coord;

};

float find_building_size(float left_x, float right_x){
   float sf = size_to_sf(left_x, right_x);
   
   float building_size_pixels = right_x - left_x;
   float building_size_cm = 0;

   building_size_cm = building_size_pixels * pixel_to_cm * sf ;

   return building_size_cm;
}

