
/* RF24 Audio Library TMRh20 2014

This sketch is intended to demonstrate how users can interact with the RF24Audio library using only the core
RF24 library. This allows users to create customized options for handling streaming audio data. For example,
a user could capture the received audio data, and feed it via USB to a pc, enabling a very low cost wireless
audio streaming solution from Arduino to PC.

Optimized RF24 Lib Source Code: https://github.com/TMRh20/RF24 
Radio Library download: https://github.com/TMRh20/RF24/archive/master.zip

References:
All Library documentation: http://tmrh20.github.io/
New (2014) RF24 Audio Library: https://github.com/TMRh20/RF24Audio
Optimized (2014) RF24 Network Lib: https://github.com/TMRh20/RF24Network

Requirements:
2 Arduinos (Uno,Nano,Mega, etc supported)
2 NRF24LO1 Radio Modules
1 or more input devices (microphone, ipod, etc)

1 or more external buttons or switches to control recording on the transmitter
OR use serial commands 
OR pre-programmed recording activation.


Setup:
1. Change the CE,CS pins below to match your chosen pins (I use 7,8 on 328 boards, and 48,49 on Mega boards)
2. Upload one of the GettingStarted or other example sketches to one device.
2. Customize and upload this sketch to the other device
3. Use as designed

Default Pin Selections:
Speaker: pins 9,10 on UNO, Nano,  pins 11,12 on Mega 2560
Input/Microphone: Analog pin A0 on all boards

Transmit button: Pin A1
VolumeUp button: Pin A2
VolumeDn button: Pin A3
Remote Transmission: Pin A4

Note: See http://arduino.cc/en/Tutorial/InputPullupSerial for info on how to wire the buttons to the pins
Note: Pin selections can be overridden by modifying the userConfig.h file included with the library
*/




#include <RF24.h>
#include <SPI.h>
#include "printf.h"

RF24 radio(7,8);		  	// Set radio up using pins 7 (CE) 8 (CS)


// These are the default multicast addresses used by the library. By simply using the
// RF24 radio library, we can interact with the RF24 Audio library directly
const uint64_t addresses[2] = { 0xABCDABCD71LL, 0x544d52687CLL};

// Note: This will only receive data broadcast to the entire radio group. Capturing private
// channel communication as well would require listening to more addresses on more pipes.


void setup() {			
  
  Serial.begin(115200);           	// Set up the serial port
  printf_begin();
  
  radio.begin();		  	// Initialize the radio  
  /* Set our radio options to the audio library defaults */
  radio.setChannel(1);            	// Set RF channel to 1
  radio.setAutoAck(0);            	// Disable ACKnowledgement packets to allow multicast reception
  radio.setCRCLength(RF24_CRC_8); 	// Only use 8bit CRC for audio
  //radio.setDataRate(RF24_1MBPS);	// Library default is RF24_1MBPS for RF24 and RF24Audio
  radio.openWritingPipe(addresses[0]); 	// Set up reading and writing pipes. 
  radio.openReadingPipe(1,addresses[1]);// All of the radios listen by default to the same multicast pipe  
  radio.printDetails();
  radio.startListening();         	// Need to start the radio listening
  
  Serial.println("Setup OK. Begin transmission of audio");
}


byte audioData[32];               	// Set up a buffer for the received data

byte samplesToDisplay = 3;        	// Change this to 32 to send the entire payload over USB/Serial

void loop() {
  
  if(radio.available()){       
      radio.read(&audioData,32);
      // Now do whatever you want with the audio data.
      // Maybe send it over USB to a PC to analyze further?
      
      for(int i=0; i<samplesToDisplay; i++){
        Serial.write(audioData[i]); 
      }
      Serial.println("");
      
     // Note: The Audio library default is 24kHZ sample rate which results in 750 payloads/second with 8bit audio
     // Decrease the audio sample rate via userConfig.h if the data stream is too fast, or increase the Serial baud rate 
  }
  
}