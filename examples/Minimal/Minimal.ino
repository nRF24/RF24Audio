
/* RF24 Audio Library TMRh20 2014

This sketch is intended to demonstrate the most basic functionality of the audio library.

Requirements:
2 Arduinos (Uno,Nano,Mega, etc supported)
2 NRF24LO1 Radio Modules
1 or more input devices (microphone, ipod, etc)
1 or more output devices (speaker, amplifier, etc)
1 or more external buttons or switches to control recording/volume/etc.

Setup:
1. Change the CE,CS pins below to match your chosen pins (I use 7,8 on 328 boards, and 48,49 on Mega boards)
2. Connect buttons as desired to the button pins listed below.
2. Upload this sketch to two or more devices
3. Use external buttons to control all devices

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
#include <RF24Audio.h>

RF24 radio(7,8);		// Set radio up using pins 7 (CE) 8 (CS)
RF24Audio rfAudio(radio,1);	// Set up the audio using the radio, and set to radio number 0.
                                // Setting the radio number is only important if one-to-one communication is desired
				// in a multi-node radio group. See the privateBroadcast() function.

void setup() {			

  rfAudio.begin();		// The only thing to do is initialize the library.

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