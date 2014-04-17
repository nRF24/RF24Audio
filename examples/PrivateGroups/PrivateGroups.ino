
/* RF24 Audio Library TMRh20 2014

This sketch is intended to demonstrate the extended multicast functionality of the audio library with
customized radio groups.

Requirements:
2+ Arduinos (Uno,Nano,Mega, etc supported)
2+ NRF24LO1 Radio Modules
1 or more input devices (microphone, ipod, etc)
2 or more output devices (speaker, amplifier, etc)
1 or more external buttons or switches to control recording/volume/etc.

Setup:
This sketch was initially used with a controller using the TMRpcm audio library to read
a WAV file from SD card, and broadcast it to the radio group(s). Any radio device can
become the transmitter with only a microphone or other input.

Default Pin Selections:
Speaker: pins 9,10 on UNO, Nano,  pins 11,12 on Mega 2560
Input/Microphone: Analog pin A0 on all boards

Note: See http://arduino.cc/en/Tutorial/InputPullupSerial for info on how to wire the buttons to the pins
Note: Pin selections can be overridden by modifying the userConfig.h file included with the library
*/


#include <RF24.h>
#include <SPI.h>
#include <RF24Audio.h>
//#include "printf.h"

RF24 radio(7,8);		// Set radio up using pins 7 (CE) 8 (CS)
RF24Audio rfAudio(radio,0);	// Set up the audio using the radio, and set to radio number 0.
                                // Setting the radio number is only important if one-to-one communication is desired
				// in a multi-node radio group. See the privateBroadcast() function.


// This is a sketch used for demonstrating the multicast capabilities of the radios
// It is designed for 5 radios, each listening on the following pipes/addresses
// With the controller broadcasting audio from SD card, it has no need to listen, so
// the 5 radios can each use all 6 pipes to listen.
/*
RadioNo:    0  1  2  3  4  Pipe:
Read Addy:  1  1  1  1  1    1    // Default config. All radios listen to the same address on pipe 1
Read Addy:  2  3  4  5  6    2    // Default config. All radios open a private pipe/addy
Read Addy:  7  7  7  8  8    0    // The rest will be configured in setup on a per radio basis
Read Addy:  9  10 8  9  9    3    // In this configuration, pipe numbers do not matter
Read Addy:  11 11 10 10 11   4    // Any radio listening on an address that is sent data will receive it
Read Addy:  13 12 12 12 13   5
*/

void setup() {	
	
  //Serial.begin(115200);
  rfAudio.begin();		// The only thing to do is initialize the library.
  //printf_begin();
  //radio.printDetails();

/** This allows custom radio groups. This radio listens on 4 additional pipes/addresses, and other
radios can listen on any similar combination of addresses. The pipe number is not important in multicast
mode, only the address. Any radios listening to the same address will receive the same audio as
broadcast to that address **/
  radio.stopListening();  
  radio.openReadingPipe(0,rfAudio.getAddress(7));  // Listen on pipe 0, using address #7
  radio.openReadingPipe(3,rfAudio.getAddress(9));  // Listen on pipe 3, using address #9
  radio.openReadingPipe(4,rfAudio.getAddress(11)); // Listen on pipe 4, using address #11
  radio.openReadingPipe(5,rfAudio.getAddress(13)); // Listen on pipe 5, using address #13
  radio.startListening();
}

void loop() {
  
  // Audio playback and button handling is all managed internally.
  // In this example, the radio is controlled by external buttons, so there is nothing to do here
  
}


/* Documentation and References:

References:
All Library documentation: http://tmrh20.github.io/
New (2014) RF24 Audio Library: https://github.com/TMRh20/RF24Audio
Optimized (2014) RF24 Lib Source Code: https://github.com/TMRh20/RF24 
Optimized (2014) RF24 Network Lib: https://github.com/TMRh20/RF24Network

*/
