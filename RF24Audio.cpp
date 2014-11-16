


#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <stddef.h>

#include "RF24Audio.h"
#include "RF24.h"
#include <userConfig.h>

//******* General Variables ************************

volatile boolean buffEmpty[2] = {true,true}, whichBuff = false, a, lCntr=0, streaming = 0, transmitting = 0;
volatile byte buffCount = 0;
volatile byte pauseCntr = 0;
unsigned int intCount = 0;
byte txCmd[2] = {'r','R'};
byte buffer[2][buffSize+1];
char volMod = -1;
byte bitPos = 0, bytePos = 25;
byte bytH;
byte radioIdentifier;

#if defined (tenBit)
  unsigned int sampl;
  byte bytL;
#endif
unsigned long volTime = 0;

RF24 radi(0,0);

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || (__AVR_ATmega32U4__) || (__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__) || (__AVR_ATmega128__) ||defined(__AVR_ATmega1281__)||defined(__AVR_ATmega2561__)
  #define rampMega
#endif

const byte broadcastVal = 255; // The value for broadcasting to all nodes using the broadcast() command.

/*****************************************************************************************************************************/
/************************************************* General Section ***********************************************************/

RF24Audio::RF24Audio(RF24& _radio, byte radioNum): radio(_radio){
	radi = radio;
	radioIdentifier = radioNum;
}

void RF24Audio::begin(){

  radio.begin();
  //delay(500);
  // Set the defined input pins as inputs with pullups high. See http://arduino.cc/en/Tutorial/InputPullupSerial
  #if defined (ENABLE_LED)
  	pinMode(ledPin,OUTPUT);
  #endif
  pinMode(speakerPin,OUTPUT); 		pinMode(speakerPin2,OUTPUT);
  pinMode(TX_PIN,INPUT_PULLUP);		pinMode(VOL_UP_PIN,INPUT_PULLUP);
  pinMode(VOL_DN_PIN,INPUT_PULLUP); pinMode(REMOTE_TX_PIN,INPUT_PULLUP);
  pinMode(REMOTE_RX_PIN,INPUT_PULLUP);

  if(SAMPLE_RATE < 16000){
    volMod = 3;
  }else{
    #if !defined (tenBit)
      volMod = 2;
    #endif
  }



  radio.setChannel(1);                 // Set RF channel to 1
  radio.setAutoAck(0);                 // Disable ACKnowledgement packets
  radio.setDataRate(RF_SPEED);         // Set data rate as specified in user options
  radio.setCRCLength(RF24_CRC_8);
  radio.openWritingPipe(pipes[0]);     // Set up reading and writing pipes. All of the radios write via multicast on the same pipe
  radio.openReadingPipe(1,pipes[1]);   // All of the radios listen by default to the same multicast pipe
  radio.openReadingPipe(2,pipes[radioIdentifier + 2]); // Every radio also has its own private listening pipe

  radio.startListening();              // NEED to start listening after opening a reading pipe
  timerStart();						   // Get the timer running
  RX();								   // Start listening for transmissions

  #if !defined (MANUAL_BUTTON_HANDLING)
  	TIMSK0 |= _BV(OCIE0B);
  #endif
}



//General functions for volume control

void vol(bool upDn){
    if(upDn==1){ volMod++;}
    else{ volMod--; }
}

void RF24Audio::volume(bool upDn){
	vol(upDn);
}

void RF24Audio::setVolume(char vol) {
    volMod = vol - 4 ;
}

void RF24Audio::timerStart(){
    ICR1 = 10 * (1600000/SAMPLE_RATE);                  //Timer will count up to this value from 0;
    TCCR1A = _BV(COM1A1) | _BV(COM1B0) | _BV(COM1B1);  //Enable the timer port/pin as output
	TCCR1A |= _BV(WGM11);                              //WGM11,12,13 all set to 1 = fast PWM/w ICR TOP
	TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);      //CS10 = no prescaling

}

#if !defined (MANUAL_BUTTON_HANDLING)

void handleButtons(){

  boolean state = digitalRead(TX_PIN);         //Get the state of the transmitting pin

  if(!state){                                   //If the pin is low, start transmitting
#if !defined (RX_ONLY)
    if(!transmitting){                         //Only do this if not already transmitting
      transmitting = 1;                        //Set the transmitting variable
      TX();                                    //Switch to TX mode
    }
  }else{                                       //If the TX pin is low
#endif
	if(transmitting){                          //If still in transmitting mode
      RX(); transmitting = 0;                  //Start receiving/Stop transmitting
    }
  }

  if(!digitalRead(VOL_UP_PIN)){                 //If the volume pin is high, raise the volume
    if(millis()-volTime > 200){                //De-bounce of buttons by 200ms
        vol(1); volTime = millis();
     }
  }else
  if(!digitalRead(VOL_DN_PIN)){
    if(millis()-volTime > 200){
        vol(0); volTime = millis();
    }
  }

  if(!digitalRead(REMOTE_TX_PIN)){              //Start remote recording and transmission of audio
    if(!transmitting){                           //If not transmitting already
        radi.stopListening();                 //Stop listening and write the request twice for good measure
        radi.write(&txCmd,2);
        radi.write(&txCmd,2);
        radi.startListening();}
        delay(200);                           //Delay as a simple button debounce
  }else
  if(!digitalRead(REMOTE_RX_PIN)){             //With TX timeout enabled, this will stop remote recording by stopping transmission for 2 seconds
        delay(2000);
  }



}


#endif //Manual button handling override


void rampDown(){
            int current = OCR1A;
            if(current > 0){
                for(int i=0; i < ICR1; i++){
	        #if defined(rampMega)
	          OCR1B = constrain((current + i),0,ICR1);
	          OCR1A = constrain((current - i),0,ICR1);
	        #else
	          OCR1B = constrain((current - i),0,ICR1);
	          OCR1A = constrain((current - i),0,ICR1);
	        #endif
	        //for(int i=0; i<10; i++){ while(TCNT1 < ICR1-50){} }
                delayMicroseconds(100);
	      }
            }

}


void rampUp(byte nextVal){

            /*int current = OCR1A;
            if(current > 0){
                for(int i=0; i < ICR1; i++){
	        #if defined(rampMega)
	          OCR1B = constrain((current - i),0,ICR1/2);
	          OCR1A = constrain((current + i),0,ICR1/2);
	        #else
	          OCR1B = constrain((current + i),0,ICR1/2);
	          OCR1A = constrain((current + i),0,ICR1/2);
	        #endif
	        //for(int i=0; i<10; i++){ while(TCNT1 < ICR1-50){} }
                delayMicroseconds(100);
	      }
            }*/

            //digitalWrite(12,HIGH);


	#if defined(rampMega)
			unsigned int resolution = ICR1;

			OCR1A = 0; OCR1B = resolution;
			for(int i=0; i < resolution; ++i){

				OCR1B = constrain(resolution-i,0,resolution);

			//if(bitRead(*TCCRnB[tt],0)){
			//	for(int i=0; i<10; i++){
			//		while(*TCNT[tt] < resolution-50){}
			//	}
			//}else{
				//delayMicroseconds(10);
				 //__asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

			//}
			}

	#endif


	byte tmp = 200;
	unsigned int mod;
	if(volMod > 0){ mod = OCR1A >> volMod; }else{ mod = OCR1A << (volMod*-1); }
	if(tmp > mod){
		for(unsigned int i=0; i<buffSize; i++){ mod = constrain(mod+1,mod, tmp); buffer[0][i] = mod; }
		for(unsigned int i=0; i<buffSize; i++){ mod = constrain(mod+1,mod, tmp); buffer[1][i] = mod; }
	}else{
		for(unsigned int i=0; i<buffSize; i++){ mod = constrain(mod-1,tmp ,mod); buffer[0][i] = mod; }
		for(unsigned int i=0; i<buffSize; i++){ mod = constrain(mod-1,tmp, mod); buffer[1][i] = mod; }
	}
	whichBuff = 0; buffEmpty[0] = 0; buffEmpty[1] = 0; buffCount = 0;

}


void RF24Audio::transmit(){
	TX();
}
void RF24Audio::receive(){
	RX();
}

#if !defined MANUAL_BUTTON_HANDLING					// Allows users to totally customize button handling or disable it
	ISR(TIMER0_COMPB_vect){						// Non-blocking interrupt vector for button management. Is triggered ~1000 times/second by default on Arduino
		handleButtons();							// Check for external button presses at the default timer0 rate
	}
#endif


uint64_t RF24Audio::getAddress(byte addressNo){

	return pipes[addressNo];

}
/*****************************************************************************************************************************/
/****************************************** Reception (RX) Section ***********************************************************/


void handleRadio(){


  if(buffEmpty[!whichBuff] && streaming){           // If in RX mode and a buffer is empty, load it
      if(radi.available() ){
		  boolean n=!whichBuff;						// Grab the changing value of which buffer is not being read before enabling nested interrupts
          TIMSK1 &= ~_BV(ICIE1);					// Disable this interrupt so it is not triggered while still running (this may take a while)
	 	  sei();									// Enable nested interrupts (Other interrupts can interrupt this one)
          radi.read(&buffer[n],32);					// Read the payload from the radio
          buffEmpty[n] = 0;                   		// Indicate that a buffer is now full and ready to play
          pauseCntr = 0;                            // For disabling speaker when no data is being received

          TIMSK1 |= _BV(ICIE1);						// Finished, re-enable the interrupt vector that runs this function
      }else{ pauseCntr++; }                         // No payload available, keep track of how many for disabling the speaker
      if(pauseCntr > 50){							// If we failed to get a payload 250 times, disable the speaker output
		  pauseCntr = 0;							// Reset the failure counter
		  rampDown();								// Ramp down the speaker (prevention of popping sounds)
		  streaming = 0;							// Indicate that streaming is stopped
		  TIMSK1 &= ~(_BV(TOIE1) );					// Disable the TIMER1 overflow vector (playback)
		  #if defined (ENABLE_LED)
		  TCCR0A &= ~_BV(COM0A1);					// Disable the TIMER0 LED visualization
		  #endif
		  TCCR1A &= ~(_BV(COM1A1) | _BV(COM1B1) | _BV(COM1B0)); // Disable speaker output
	  }
  }else

  if(!streaming){                  					// If not actively reading a stream, read commands instead
    if(radi.available() ){							// If a payload is available
		TIMSK1 &= ~_BV(ICIE1);						// Since this is called from an interrupt, disable it
	 	sei();										// Enable global interrupts (nested interrupts) Other interrupts can interrupt this one
        radi.read(&buffer[0],32);					// Read the payload into the buffer
        switch(buffer[0][0]){                       // Additional commands can be added here for controlling other things via radio command
          #if !defined (RX_ONLY)
          case 'r': if(buffer[0][1] == 'R' && radioIdentifier < 2){  //Switch to TX mode if we received the remote tx command and this is radio 0 or 1
                      TX();
                    }
                    break;
          #endif
          default: streaming= 1;              		// If not a command, treat as audio data, enable streaming


   				   TCCR1A |= _BV(COM1A1) | _BV(COM1B1) | _BV(COM1B0);  //Enable output to speaker pin
   				   rampUp(buffer[0][31]);						// Ramp up the speakers to prevent popping
   				   //buffEmpty[0] = false;			// Set the status of the memory buffers
   				   //buffEmpty[1] = true;
          		   TIMSK1 |= _BV(TOIE1);			// Enable the overflow vector
          		   #if defined (ENABLE_LED)
          		   TCCR0A |= _BV(COM0A1);			// Enable the LED visualization output
          		   #endif
				   break;

        }
        TIMSK1 |= _BV(ICIE1);						// Finished: Re-enable the interrupt that runs this function.
    }
  }
}



void RX(){ 											// Start Receiving

		TIMSK1 &= ~_BV(OCIE1B) | _BV(OCIE1A);		// Disable the transmit interrupts
		ADCSRA = 0; ADCSRB = 0;                		// Disable Analog to Digital Converter (ADC)
        buffEmpty[0] = 1; buffEmpty[1] = 1;    		// Set the buffers to empty
        #if defined (oversampling)					// If running the timer at double the sample rate
          ICR1 = 10 * (800000/SAMPLE_RATE);    		// Set timer top for 2X oversampling
        #else
          ICR1 = 10 * (1600000/SAMPLE_RATE);		// Timer running at normal sample rate speed
        #endif

        radi.openWritingPipe(pipes[0]);       		// Set up reading and writing pipes
        radi.openReadingPipe(1,pipes[1]);
        radi.startListening();                		// Exit sending mode
        TIMSK1 = _BV(ICIE1);                  		// Enable the capture interrupt vector (handles buffering and starting of playback)



}

boolean nn = 0;
volatile byte bufCtr = 0;
volatile unsigned int visCtr = 0;

ISR(TIMER1_CAPT_vect){							   // This interrupt checks for data at 1/16th the sample rate. Since there are 32 bytes per payload, it gets two chances for every payload

bufCtr++; visCtr++;								   // Keep track of how many times the interrupt has been triggered

if(bufCtr >= 16){								   // Every 16 times, do this
	handleRadio();							   	   // Check for incoming radio data if not transmitting

	bufCtr = 0;									   // Reset this counter

	if(visCtr >= 32 && streaming){			   	   // Run the visualisation at a much slower speed so we can see the changes better
		OCR0A = buffer[whichBuff][0] << 2;		   // Adjust the TIMER0 compare match to change the PWM duty and thus the brightess of the LED
		visCtr = 0;								   // Reset the visualization counter
	}

}


}


// Receiving interrupt
ISR(TIMER1_OVF_vect){                                      				// This interrupt vector loads received audio samples into the timer register


  if(buffEmpty[whichBuff] ){ whichBuff=!whichBuff; }else{  				// Return if both buffers are empty

    #if defined (oversampling)											// If running the timers at 2X speed, only load a new sample every 2nd time
      if(lCntr){lCntr = !lCntr;return;}   lCntr=!lCntr;
    #endif


	#if !defined (tenBit)
/*************** Standard 8-Bit Audio Playback ********************/
  	  if(volMod < 0 ){                                         			 //Load an audio sample into the timer compare register
  		  OCR1A = OCR1B = (buffer[whichBuff][intCount] >> volMod*-1);    //Output to speaker at a set volume
  	  }else{
		  OCR1A = OCR1B = buffer[whichBuff][intCount] << volMod;
  	  }

  	  intCount++;                                              			 //Keep track of how many samples have been loaded

  	  if(intCount >= buffSize){                                			 //If the buffer is empty, do the following
  		  intCount = 0;                                        			 //Reset the sample count
  		  buffEmpty[whichBuff] = true;                        			 //Indicate which buffer is empty
  		  whichBuff = !whichBuff;                              			 //Switch buffers to read from
  	  }

/*************** 10 - bit Audio *************************************/
	#else

	  sampl = buffer[whichBuff][intCount];								  // Load 8bits of the sample into a temporary buffer
	  bitWrite(  sampl, 8,  bitRead( buffer[whichBuff][bytePos],bitPos)); // Read the 9th bit, and write it to the temporary buffer
	  bitPos++;															  // Keep track of the bit-position, since bits are loaded in bytes 25-31
	  bitWrite(sampl, 9, bitRead(buffer[whichBuff][bytePos],bitPos));	  // Read the 10th bit, and write it to the temporary buffer
	  bitPos++;															  // Keep track of the bit-position

	  if(volMod < 0 ){                                         			  // Load an audio sample into the timer compare register
	      OCR1A = OCR1B = sampl >> (volMod*-1);        					  // Output to speaker at a set volume
	  }else{
	      OCR1A = OCR1B = sampl << volMod;
	  }


	  if(bitPos >=8){													  // If 8 bits have been read, reset the counter and switch to the next byte
	      bitPos = 0;
	      bytePos = bytePos+1;
	  }

      intCount++;                                            			  // Keep track of how many samples have been loaded

	  if(intCount >= 25){                                    			  // If the buffer is empty, do the following
	      bytePos = 25;
	      bitPos = 0;
	      intCount = 0;                                        			  // Reset the sample count
	      buffEmpty[whichBuff] = true;                         			  // Indicate which buffer is empty
	      whichBuff = !whichBuff;                              			  // Switch buffers to read from
	  }


	#endif
  }
}



/*****************************************************************************************************************************/
/*************************************** Transmission (TX) Section ***********************************************************/

#if !defined (RX_ONLY) //If TX is enabled:

void RF24Audio::broadcast(byte radioID){

	if(radioID == radioIdentifier){ return; }						// If trying to send to our own address, return

	noInterrupts();													// Disable interrupts during change of transmission address

	if(radioID == broadcastVal){
			radio.openWritingPipe(pipes[1]);						// Use the public multicast pipe
	}else{
			radio.openWritingPipe(pipes[radioID + 2]);				// Open a pipe to the specified radio(s). If two or more radios share the same ID,
	}																// they will receive the same single broadcasts, but will not be able to initiate
	interrupts();													// private communication betwen each other
}


  //Transmission sending interrupt
  ISR(TIMER1_COMPA_vect){                                    		 // This interrupt vector sends the samples when a buffer is filled
	 if(buffEmpty[!whichBuff] == 0){                      			 // If a buffer is ready to be sent
	 		a = !whichBuff;                      					 // Get the buffer # before allowing nested interrupts
   			TIMSK1 &= ~(_BV(OCIE1A));            					 // Disable this interrupt vector
   			sei();                               					 // Allow other interrupts to interrupt this vector (nested interrupts)
            //radi.startFastWrite(&buffer[a],32);
			radi.writeFast(&buffer[a],32);
			buffEmpty[a] = 1;                    					 // Mark the buffer as empty
	   	    TIMSK1 |= _BV(OCIE1A);                					 // Re-enable this interrupt vector
     }

  }




  //Transmission buffering interrupt
  ISR(TIMER1_COMPB_vect){                                            // This interrupt vector captures the ADC values and stores them in a buffer

    #if !defined (tenBit)                                            // 8-bit samples

      buffer[whichBuff][buffCount] = bytH = ADCH;                    // Read the high byte of the ADC register into the buffer for 8-bit samples

      #if defined (speakerTX)                                        // If output to speaker while transmitting is enabled
        if(volMod < 0 ){  OCR1A = bytH >> (volMod*-1);       		 // Output to speaker at a set volume if defined
		}else{  	  OCR1A = bytH << volMod;
		}
      #endif

    #else                                                            // 10-bit samples are a little more complicated, but offer better quality for lower sample rates
      buffer[whichBuff][buffCount] = bytL = ADCL;                    // In 10-bit mode, the ADC register is right-adjusted, we need to read the low, then high byte each time
      bytH = ADCH;
      bitWrite(buffer[whichBuff][bytePos],bitPos, bitRead(bytH,0));  // The low bytes are stored in the first 25 bytes of the payload. The additional 2 bits are stored in
      bitWrite(buffer[whichBuff][bytePos],bitPos+1, bitRead(bytH,1));// pairs in the remaining bytes #25 to 31. Read the first and 2nd bits of the high register into the payload.
      bitPos+=2;
      if(bitPos >= 8){ bitPos = 0; bytePos = bytePos+1; }            // Every time a byte is full, increase byte position by 1 and reset the bit count.

      #if defined (speakerTX)                                        // If output to speaker while transmitting is enabled
        sampl = bytL;                                                // Load the two bytes into the 2-byte unsigned integer. Low byte first
        sampl |= bytH << 8;                                          // Shift the high byte 8bits and load it into the unsigned int using an OR comparison
        if(volMod < 0 ){  OCR1A = sampl >> (volMod*-1);      		 // Output to speaker at a set volume if defined
		}else{  	  OCR1A = sampl << volMod;
		}
      #endif

    #endif




    buffCount++;                                         // Keep track of how many samples have been loaded

    #if !defined (tenBit)                                // 8-bit mode
	if(buffCount >= 32){                                 // In 8-bit mode, do this every 32 samples

    #else                                                // 10-bit mode
    if(buffCount >= 25){                            	 // In 10-bit mode, do this every 25 samples
        bytePos = 25;                                    // Reset the position for the extra 2 bits to the 25th byte
        bitPos = 0;                                      // Reset the bit position for the extra 2 bits
    #endif

    	//Both modes
	  	buffCount = 0;                                   // Reset the sample counter
      	buffEmpty[!whichBuff] = 0;                       // Indicate which bufffer is ready to send
  	  	whichBuff = !whichBuff;                          // Switch buffers and load the other one
  	}
  }


void TX(){

		TIMSK1 &= ~(_BV(ICIE1) | _BV(TOIE1));				 // Disable the receive interrupts
		#if defined (ENABLE_LED)
		TCCR0A &= ~_BV(COM0A1);								 // Disable LED visualization
		#endif
        radi.openWritingPipe(pipes[1]);   				 	 // Set up reading and writing pipes
        radi.openReadingPipe(1,pipes[0]);
        radi.stopListening();							 	 // Enter transmit mode on the radio


        streaming = 0;
  		buffCount = 0; buffEmpty[0] = 1; buffEmpty[1] = 1;   //Set some variables


        byte pin = ANALOG_PIN;
	/*** This section taken from wiring_analog.c to translate between pins and channel numbers ***/
	#if defined(analogPinToChannel)
	#if defined(__AVR_ATmega32U4__)
		if (pin >= 18) pin -= 18; // allow for channel or pin numbers
	#endif
		pin = analogPinToChannel(pin);
	#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		if (pin >= 54) pin -= 54; // allow for channel or pin numbers
	#elif defined(__AVR_ATmega32U4__)
		if (pin >= 18) pin -= 18; // allow for channel or pin numbers
	#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
		if (pin >= 24) pin -= 24; // allow for channel or pin numbers
	#else
		if (pin >= 14) pin -= 14; // allow for channel or pin numbers
	#endif

	#if defined(ADCSRB) && defined(MUX5)
		// the MUX5 bit of ADCSRB selects whether we're reading from channels
		// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
		ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
	#endif

	#if defined(ADMUX)
		ADMUX = (pin & 0x07) | _BV(REFS0); 									//Enable the ADC PIN and set 5v Analog Reference
	#endif

        ICR1 = 10 * (1600000/SAMPLE_RATE);           						//Timer counts from 0 to this value

        #if !defined (speakerTX) 											//If disabling/enabling the speaker, ramp it down
        	rampDown();
            TCCR1A &= ~(_BV(COM1A1));                    					//Disable output to speaker

        #endif
      TCCR1A &= ~(_BV(COM1B1) |_BV(COM1B0) );

      #if !defined (tenBit)
	ADMUX |= _BV(ADLAR);            										//Left-shift result so only high byte needs to be read
      #else
        ADMUX &= ~_BV(ADLAR);           									//Don't left-shift result in 10-bit mode
      #endif

        ADCSRB |= _BV(ADTS0) | _BV(ADTS0) | _BV(ADTS2);           //Attach ADC start to TIMER1 Capture interrupt flag
	byte prescaleByte = 0;

	if(      SAMPLE_RATE < 8900){  prescaleByte = B00000111;} //128
        else if( SAMPLE_RATE < 18000){ prescaleByte = B00000110;} //ADC division factor 64 (16MHz / 64 / 13clock cycles = 19230 Hz Max Sample Rate )
	else if( SAMPLE_RATE < 27000){ prescaleByte = B00000101;} //32  (38461Hz Max)
	else if( SAMPLE_RATE < 65000){ prescaleByte = B00000100;} //16  (76923Hz Max)
        else   {                       prescaleByte = B00000011;} //8  (fast as hell)

        ADCSRA = prescaleByte;                        //Adjust sampling rate of ADC depending on sample rate
	ADCSRA |= _BV(ADEN) | _BV(ADATE);             //ADC Enable, Auto-trigger enable


       TIMSK1 = _BV(OCIE1B) | _BV(OCIE1A);          //Enable the TIMER1 COMPA and COMPB interrupts
}


#endif