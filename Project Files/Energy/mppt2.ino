#include <Wire.h>
#include <INA219_WE.h>
#include <SPI.h>
#include <SD.h>

INA219_WE ina219; // this is the instantiation of the library for the current sensor

Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 10; //hardwired chip select for the SD card
float vpd,vb,vref,va ,iL,duty_cycle,current_mA,pin,pnext,pout,efficiency; // Measurement Variables
unsigned int sensorValue0,sensorValue1,sensorValue2,sensorValue3;  // ADC sample values declaration
float ev=0,cv=0,ei=0,oc=0,a=0.05,eP=0,pin_previous=0,vprevious=0,iL_previous=0; //internal signals
float Ts=0.0001; //1.25 kHz control frequency. It's better to design the control period as integral multiple of switching period.
float kpv=0.05024,kiv=15.78,kdv=0; // voltage pid.
float u0v,u1v,delta_uv,e0v,e1v,e2v; // u->output; e->error; 0->this time; 1->last time; 2->last last time
float kpi=0.02512,kii=39.4,kdi=0; // current pid.
float u0i,u1i,delta_ui,e0i,e1i,e2i; // Internal values for the current controller
float uv_max=4, uv_min=0; //anti-windup limitation
float ui_max=1, ui_min=0; //anti-windup limitation
float current_limit = 1.0;



unsigned int int_count = 0;
unsigned int loopTrigger;
unsigned int com_count=0;   // a variables to count the interrupts. Used for program debugging.

void setup() {

  //Basic pin setups
  
  noInterrupts(); //disable all interrupts
  pinMode(13, OUTPUT);  //Pin13 is used to time the loops of the controller
  pinMode(3, INPUT_PULLUP); //Pin3 is the input from the Buck/Boost switch
  pinMode(2, INPUT_PULLUP); // Pin 2 is the input from the CL/OL switch
  analogReference(EXTERNAL); // We are using an external analogue reference for the ADC----now using Vdd
  // TimerA0 initialization for control-loop interrupt.
  
  TCA0.SINGLE.PER = 999; //
  TCA0.SINGLE.CMP1 = 999; //
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm; //16 prescaler, 1M.
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP1_bm; 

  // TimerB0 initialization for PWM output
  
  pinMode(6, OUTPUT);
  TCB0.CTRLA=TCB_CLKSEL_CLKDIV1_gc | TCB_ENABLE_bm; //62.5kHz
  analogWrite(6,120); 

  interrupts();  //enable interrupts.
  Wire.begin(); // We need this for the i2c comms for the current sensor
  ina219.init(); // this initiates the current sensor
  Wire.setClock(700000); // set the comms speed for i2c
  Serial.begin(9600);

  Serial.println("\nInitializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("* is a card inserted?");
    while (true) {} //It will stick here FOREVER if no SD is in on boot
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  if (SD.exists("SD_Test.csv")) { // Wipe the datalog when starting
    SD.remove("SD_Test.csv");
  }
}

 void loop() {
  if(loopTrigger) { // This loop is triggered, it wont run unless there is an interrupt
    digitalWrite(13, HIGH);// set pin 13. Pin13 shows the time consumed by each control cycle. It's used for debugging.
    int_count++;
    digitalWrite(13, LOW); // reset pin13.
    loopTrigger = 0;
   }
    
    if(int_count==1000){//every one second
    sampling();
    
    if((-current_mA)<=5){
     duty_cycle=duty_cycle;
   }

    
    pin=vb*(-current_mA);
    String datavalue=String(pin) +","+ String(-current_mA) +"," + String(vb)+","+String(va)+","+String(duty_cycle);
    //String datavalue=String(pin) +" "+ String(-current_mA)+" "+ String(vb)+" "+ String(duty_cycle);
    //Serial.println(pin);
    //Serial.println("vb: ");
    //Serial.println(vb);
    //Serial.println(-current_mA);
    //Serial.println("vout: ");
    //Serial.println(va);
    if(pin>pin_previous){
       if(vb>vprevious){
         duty_cycle=duty_cycle-a;
       }
       else{
          duty_cycle=duty_cycle+a;
        }
      }
     else{
      if(vb>vprevious){
      duty_cycle=duty_cycle+a;
      }
      else{
      duty_cycle=duty_cycle-a;
      } 
     }
     duty_cycle=saturation(duty_cycle,0.99,0.10);
     vprevious=vb;
     pin_previous=pin;
     pwm_modulate(duty_cycle);
     int_count=0;
     
    Serial.println(datavalue); // send it to serial as well in case a computer is connected
    File dataFile = SD.open("SD_Test.csv", FILE_WRITE); // open our CSV file
      if (dataFile){ //If we succeeded (usually this fails if the SD card is out)
        dataFile.println(datavalue); // print the data
      } 
      else {
      Serial.println("File not open"); //otherwise print an error
      }
    dataFile.close(); // close the file
  }
  
  
}


// Timer A CMP1 interrupt. Every 800us the program enters this interrupt. 
// This, clears the incoming interrupt flag and triggers the main loop.

ISR(TCA0_CMP1_vect){
  TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_CMP1_bm; //clear interrupt flag
  loopTrigger = 1;
}

// This subroutine processes all of the analogue samples, creating the required values for the main loop

void sampling(){

  // Make the initial sampling operations for the circuit measurements
  
  sensorValue0 = analogRead(A0); //sample Vb
  sensorValue2 = analogRead(A2); //sample Vref
  sensorValue3 = analogRead(A3); //sample Vpd
  current_mA = ina219.getCurrent_mA(); // sample the inductor current (via the sensor chip)

  // Process the values so they are a bit more usable/readable
  // The analogRead process gives a value between 0 and 1023 
  // representing a voltage between 0 and the analogue reference which is 4.096V
  
  vb = sensorValue0 * (4.096 / 1023.0)*(129000.0/20000.0); // Convert the Vb sensor reading to volts with potential divider
  vref = sensorValue2 * (4.096 / 1023.0); // Convert the Vref sensor reading to volts
  vpd = sensorValue3 * (4.096 / 1023.0); // Convert the Vpd sensor reading to volts
  va=(vpd*890)/330;
  iL = -current_mA/1000.0;
  // The inductor current is in mA from the sensor so we need to convert to amps.
  // We want to treat it as an input current in the Boost, so its also inverted
  // For open loop control the duty cycle reference is calculated from the sensor
  // differently from the Vref, this time scaled between zero and 1.
  // The boost duty cycle needs to be saturated with a 0.33 minimum to prevent high output voltages
  
 
  
}

float saturation( float sat_input, float uplim, float lowlim){ // Saturatio function
  if (sat_input > uplim) sat_input=uplim;
  else if (sat_input < lowlim ) sat_input=lowlim;
  else;
  return sat_input;
}

void pwm_modulate(float pwm_input){ // PWM function
  analogWrite(6,(int)(pwm_input*255)); 
}

// This is a PID controller for the voltage



// This is a PID controller for the current



/*end of the program.*/
