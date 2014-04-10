
/* RF24 Audio Library TMRh20 2014

This sketch is intended to demonstrate the basic functionality of the audio library.

Requirements:
2 Arduinos (Uno,Nano,Mega, etc supported)
2 NRF24LO1 Radio Modules
1 or more input devices (microphone, ipod, etc)
1 or more output devices (speaker, amplifier, etc)

Setup:
1. Change the CE,CS pins below to match your chosen pins (I use 7,8 on 328 boards, and 48,49 on Mega boards)
2. Upload this sketch to two or more devices
3. Send serial commands via the serial monitor to control transmission and volume (volume only affects receiving devices)

Default Pin Selections:
Speaker: pins 9,10 on UNO, Nano,  pins 11,12 on Mega 2560
Input/Microphone: Analog pin A0 on all boards

*/


#include <RF24.h>
#include <SPI.h>
#include <RF24Audio.h>
#include "printf.h"		// General includes for radio and audio lib

RF24 radio(7,8);		// Set radio up using pins 7 (CE) 8 (CS)
RF24Audio rfAudio(radio,1);	// Set up the audio using the radio, and set to radio number 0

void setup() {			
  Serial.begin(115200);		// Enable Arduino serial library
  
  printf_begin();               // Radio library uses printf to output debug info  
  radio.begin();                // Must start the radio here, only if we want to print debug info
  radio.printDetails();         // Print the info
  
  rfAudio.begin();		// Start up the radio and audio libararies
}

void loop() {

    if(Serial.available()){                                           // Receive and analyze incoming serial data
        switch(Serial.read()){
            case 'r': rfAudio.transmit(); break;  		      // Send an r or an s over serial to control playback
            case 's': rfAudio.receive();  break;
            case '=': rfAudio.volume(1);  break;                      // Control volume by sending = or - over serial
            case '-': rfAudio.volume(0);  break;
        }
    }
    Serial.println("test");
    delay(1000);
}


/* Documentation and References:

References:
All Library documentation: http://tmrh20.github.io/
New (2014) RF24 Audio Library: https://github.com/TMRh20/RF24Audio
Optimized (2014) RF24 Lib Source Code: https://github.com/TMRh20/RF24 
Optimized (2014) RF24 Network Lib: https://github.com/TMRh20/RF24Network

*/
