/**************INCLUDE ADAFRUIT LIBRARIES FOR DAC********************/
#include <Wire.h>                     // bunch of code to communicate with DAC
#include <Adafruit_MCP4725.h>         // more code to use the DAC from adafruit. 

/****************************DEFINE VARIABLES************************/
volatile long EncoderCount = 0;        // variable to keep track of number of pulses sent by the encoder
int ChannelA;                          // flag variables for channel A and B
int ChannelB;
float DiscAngle, PreviousDiscAngle;    // define the DiscAngle variables
float DiscSpeed;                       // define the DiscSpeed variable
unsigned long Time, PreviousTime ;     // define the time variables
 
Adafruit_MCP4725 mydac;                // an instance of adafruit DAC class 
int DACvalue = 0;                      // voltage given to Motor controller voltage by the DAC = 5*DACvalue/4095  

/*******************************************************************/
void setup() {
  // put your setup code here, it will run once at the beginning:
  Serial.begin(38400); // start serial communication through USB 
  
  pinMode(2, INPUT); // define encoder pins as inputs
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), checkChannelA, CHANGE); // start interrupts on the encoder pins. 
  attachInterrupt(digitalPinToInterrupt(3), checkChannelB, CHANGE); 

  mydac.begin(0x62);                 // setup the DAC board
  mydac.setVoltage(0,false);         // set DAC voltage value to 0, to stop the motor at beginning. 
  delay(3000);                       // pause the code just to give you some time at the start after you upload the code

   // initialize the 'previous' variables before starting the loop
  PreviousDiscAngle = EncoderCount*2.0*M_PI/1024.0;           // initialize the disc angle variable
  PreviousTime = millis();                                    // initialize the time variable
  delay(10);                                                  // wait a bit
}

void loop() {
  // put your main code here, it will run repeatedly:
        DACvalue = 2000; 
        mydac.setVoltage(DACvalue,false);                   // set DAC voltage value. Make it such that the final disc speed is in the provided range. 
        
        DiscAngle = EncoderCount*2.0*M_PI/1024.0;           // convert encoder counts to angle
        Time = millis();                                    // ask arduino for time in milliseconds
        DiscSpeed =  (DiscAngle-PreviousDiscAngle)/((Time-PreviousTime)/1000.0);      // This is numerical differentiation  (get speed in rad/s)

  // print stuff on serial monitor
        Serial.print(Time);                                 // print time in ms
        Serial.print(" ");                                  // print space
        Serial.print(DiscSpeed);                            // print disc speed in rad/s
        Serial.print(" ");                                  // print space
        Serial.println(DACvalue);                           // print DAC 'voltage' value

  // update the 'previous' variables before starting the next loop
       PreviousDiscAngle = DiscAngle;
       PreviousTime      = Time;
       delay(10);                                          // pause just to make loop time about 10ms for consistency.
 
  }
      

   
/*************** Functions to count encoder ticks*****************/
void checkChannelA() {

  // Low to High transition?
  if (digitalRead(2) == HIGH) {
    ChannelA = 1;
    if (!ChannelB) {
      EncoderCount = EncoderCount + 1;

    }
  }

  // High-to-low transition?
  if (digitalRead(2) == LOW) {
    ChannelA = 0;
  }

}


// Interrupt on B changing state
void checkChannelB() {

  // Low-to-high transition?
  if (digitalRead(3) == HIGH) {
    ChannelB = 1;
    if (!ChannelA) {
      EncoderCount = EncoderCount - 1;
    }
  }

  // High-to-low transition?
  if (digitalRead(3) == LOW) {
    ChannelB = 0;
  }
}
