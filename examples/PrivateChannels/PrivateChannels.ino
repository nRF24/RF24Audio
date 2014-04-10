
/* RF24 Audio Library TMRh20 2014

This sketch is demonstrates the use of private channels (node-to-node) in a multi-radio group.

Requirements:
3 or more Arduinos (Uno,Nano,Mega, etc supported)
3 or more NRF24LO1 Radio Modules
1 or more input devices (microphone, ipod, etc)
2 or more output devices (speaker, amplifier, etc)

Setup:
1. Change the CE,CS pins below to match your chosen pins (I use 7,8 on 328 boards, and 48,49 on Mega boards)
2. Set the radio identifier number. This will identify each radio in the radio group.
2. Upload this sketch to three or more devices
3. Send commands via the serial monitor to control the devices

Default Pin Selections:
Speaker: pins 9,10 on UNO, Nano,  pins 11,12 on Mega 2560
Input/Microphone: Analog pin A0 on all boards

Note: Should support up to 10 radios currently. This has not been fully tested.
Note: Pin selections can be overridden by modifying the userConfig.h file included with the library
Note: If two or more radios share the same identifier, they will receive the same single-pipe transmissinos, but will be unable
to establish node-to-node communication between each other.

*/


#include <RF24.h>
#include <SPI.h>
#include <RF24Audio.h>

RF24 radio(7,8);		// Set radio up using pins 7 (CE) 8 (CS)

/********* Set the Radio Identifier Here ************/
RF24Audio rfAudio(radio,0);	// Set up the audio using the radio, and set to radio number 0.
                                // Setting the radio number is only important if one-to-one communication is desired
                                // in a multi-node radio group. See the private

void setup() {			

  rfAudio.begin();		// The only thing to do is initialize the library.

}

void loop() {
  
  if(Serial.available()){
      switch(Serial.read()){
          case 'r': rfAudio.transmit();  break;
          case 's': rfAudio.receive();   break;
          case '1': rfAudio.broadcast(1);   break; // Broadcast to radio number 1 only ( This one being radio number 0 )
          case '2': rfAudio.broadcast(2);   break; // Broadcast to radio number 2 only
          case '3': rfAudio.broadcast(255); break; // Switch back to multicast to all radios in the group
        } 
    }
  
}


/* Documentation and References:

References:
All Library documentation: http://tmrh20.github.io/
New (2014) RF24 Audio Library: https://github.com/TMRh20/RF24Audio
Optimized (2014) RF24 Lib Source Code: https://github.com/TMRh20/RF24 
Optimized (2014) RF24 Network Lib: https://github.com/TMRh20/RF24Network

*/