/*
TMRh20 2016

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/** RPi audio broadcast to RF24Audio nodes
 *
 * Reads from WAV file and broadcasts audio
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <RF24/RF24.h>
#include <unistd.h>
#include <fstream>

using namespace std;

/****************** Raspberry Pi ***********************/

RF24 radio(25,0);

// File should be 16 khz sample rate, mono, wav format
char audioFile[] = "houston24.wav";

/**************************************************************/

// The addresses used in RF24Audio
const uint64_t pipes[14] = { 0xABCDABCD71LL, 0x544d52687CLL, 0x544d526832LL, 0x544d52683CLL,0x544d526846LL, 0x544d526850LL,0x544d52685ALL, 0x544d526820LL, 0x544d52686ELL, 0x544d52684BLL, 0x544d526841LL, 0x544d526855LL,0x544d52685FLL,0x544d526869LL};


int main(int argc, char** argv){

  cout << "RF24Audio On Rpi\n";

  // Configure radio settings for RF24Audio
  radio.begin(); 
  radio.setChannel(1);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(0); 
  radio.setCRCLength(RF24_CRC_8);

  radio.openWritingPipe(pipes[1]);
  radio.startListening();
  radio.printDetails();
  radio.stopListening();

  
  uint32_t sampleDelay = 2000;
  
  // Read the entire wav file into memory
  streampos size;
  char * memblock;
  
  ifstream file (audioFile, ios::in|ios::binary|ios::ate);
  if (file.is_open()){
    size = file.tellg();
    memblock = new char [size];
    file.seekg (0, ios::beg);
    file.read (memblock, size);
    file.close();
    cout << "the entire file content is in memory\n";   
    
  }else{
      exit(1);
  }
  
  // Get the sample rate of the wav file
    uint32_t sampleRate = memblock[24];
    sampleRate |= memblock[25] << 8;
    sampleRate |= memblock[26] << 16;
    sampleRate |= memblock[27] << 24;    
    cout << "Sample Rate " << sampleRate << "\n";      
    
    // Calculate the required delay based on sample rate
    sampleDelay = (1000000.0 / sampleRate * 32) - 1;
    cout << "Sample Delay " << sampleDelay << "\n";    
    
    
    // Data starts at byte 44, after the .wav header as long as there is no metadata
    int32_t filePos = 44;    
    cout << "Output via radio...\n";    
    radio.writeFast(&memblock[filePos],32);
    filePos+=32;
    
    while (1){        
     
      radio.writeFast(&memblock[filePos],32);
      delayMicroseconds(sampleDelay);
      filePos+=32;
      if(filePos >= size){
        break;          
      }
	  

} // loop

cout << "Complete, exit...\n";  
delete[] memblock;

} // main






