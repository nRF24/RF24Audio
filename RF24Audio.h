
/* RFAudio Library: by TMRh20 2011-2014*/

#ifndef __RF24Audio_H__
#define __RF24Audio_H__

/**
 * @file RF24Audio.h
 *
 * Class declaration for RF24Audio Library
 */

class RF24;

/**
 * TMRh20 2014 - RF24Audio: Arduino Realtime Audio Streaming library
 *
 * This class implements an Audio Streaming library using nRF24L01(+) radios driven
 * by the Optimized RF24 library. https://github.com/TMRh20/RF24
 */

class RF24Audio
{

public:
  /**
   * Setup the radio and radio identifier
   * @note Changing radioNum is only required if utilizing private node-to-node communication as
   * opposed to broadcasting to the entire radio group
   *
   * @code
   *	RF24 radio(48,49);				// Initialize the radio driver
   *	RF24Audio rfAudio(radio,0);		// Initialize the audio driver
   * @endcode
   *
   * @param _radio   The underlying radio driver instance
   * @param radioNum The radio identifier
   */
	RF24Audio(RF24& _radio, byte radioNum);

  /**
   * Initialize the radio and audio library
   *
   * Generally called in setup to initialize the radio
   * @code
   *	rfAudio.begin();
   * @endcode
   */
	void begin();

  /**
   * Volume Control
   * @code
   *	rfAudio.volume(1); // Raise the volume
   * @endcode
   * @param upDn Set 0 to lower volume, 1 to raise volume
   *
   */
	void volume(bool upDn);

  /**
   * Volume Control
   * @code
   *	rfAudio.setVolume(4); // Set the volume to mid-level
   * @endcode
   * @param vol Set at 0 to 7 for range of volume control
   *
   */
	void setVolume(char vol);

  /**
   * Control transmission through code
   * @code
   *	rfAudio.transmit(); // Begin realtime audio streaming
   * @endcode
   * Call this function to begin transmission
   *
   */
	void transmit();

  /**
   * Stop transmission through code
   * @code
   *	rfAudio.receive(); // Stop audio streaming
   * @endcode
   * Call this function to stop transmission
   *
   */
	void receive();

  /**
   * Control of Private or Public Communication
   *
   * Call this function to establish private communication between nodes
   * in a radio group, or to switch back to public transmission.
   * @note Using a radioID of 255 will disable private communication and broadcast to all nodes
   *
   * @code
   *	rfAudio.broadcast(1); 	// Only transmit audio to radio number 1
   *	rfAudio.broadcast(255);  // Transmit audio to all radios in the group
   * @endcode
   * @param radioID  Set the radioID of the radio to communicate privately with.
   */
	void broadcast(byte radioID);

   /**
   * Get any of the preset radio addresses
   *
   * Useful for listening nodes who wish to create private or additional radio groups
   * The library has 14 predefined radio addreses. All radios listen/write on the first
   * two addresses (0,1), and engage a private channel based on the radio number.
   * Radio 0 listens on address 2, Radio 1 on address 3, etc.
   *
   * @code
   *	uint64_t newAddress = rfAudio.getAddress(3);	// Gets the 3rd defined radio address
   *    OR
   *	radio.openReadingPipe(0,rfAudio.getAddress(7)); // Listens on the 7th defined radio address
   * @endcode
   * @param addressNo  Numbers 0 through 14 to access any part of the defined address array
   * @return RadioAddress:  Returns the requested predefined radio address
   */
    uint64_t getAddress(byte addressNo);


private:
	RF24& radio;
	void timerStart();
};

void TX();
void RX();

#endif


/**
 * @example GettingStarted.ino
 *
 * This sketch is intended to demonstrate the basic functionality of the audio library.
 *
 */

 /**
  * @example Minimal.ino
  * This sketch is intended to demonstrate the most basic functionality of the audio library.
 */

 /**
  * @example PrivateChannels.ino
  * This sketch is demonstrates the use of private channels (node-to-node) in a multi-radio group.
  *
 */

 /**
  * @example PrivateGroups.ino
  * This sketch is demonstrates the use of private groups (node-to-custom_groups) in a multi-radio group.
  *
 */

 /**
  * @example USB_Audio.ino
  * This sketch is demonstrates how to interact with the audio library directly using the core
  * RF24 radio library: http://tmrh20.github.io/RF24/
  *
 */

 /**
  * @mainpage RF24Audio - Realtime Audio Streaming Library for Arduino
  *
  * This class implements a realtime audio streaming solution using nRF24L01(+) radios driven
  * by the newly optimized <a href="http://tmrh20.github.com/RF24/">RF24</a> library fork.
  *
  * @section Features Features
  * @li <b>User friendly setup and configuration:</b> For beginners too: Just connect a radio module, microphone, and speaker. The library handles the rest.
  * @li <b>Recording and broadcasting of audio</b> to multiple devices using only Arduino, RF24 modules and input/output devices (speaker,microphone)
  * @li <b>Multicast:</b> Enables broadcasting to all nodes, single nodes, or partial groups of nodes
  * @li <b>External controls:</b> Use external buttons or code to control audio streaming
  * @li <b>Volume control:</b> Use external buttons or code to control audio volume on receiving devices.
  * @li <b>Remote control:</b> Start recording remotely via radio commands (Currently cannot stop remote recording)
  * @li <b>LED Indicator/Visualization:</b> Indicates audio playback and amplitude.
  * @li <b>Customization:</b> Using the underlying RF24 core library allows custom interaction with audio devices running this library. Receive audio data
  *        and stream it to a PC over USB, create and broadcast computer generated audio in realtime, and more! See the <a href="examples.html">USB_Audio example</a> for more info.
  * @li <b>Create additional node groups:</b> Allows nodes to join private broadcast groups, and multicast only within their group as desired. See advanced section below.
  * @section More How to learn more
  *
  * @li <a href="classRF24Audio.html">Library Class Documentation</a>
  * @li <a href="http://tmrh20.github.com/RF24/">RF24: Underlying radio driver (2014 - Newly Optimized)</a>
  * @li <a href="http://tmrh20.blogspot.com/2014/03/high-speed-data-transfers-and-wireless.html">My Blog: RF24 Optimization Overview</a>
  * @li <a href="http://tmrh20.blogspot.com/2014/03/arduino-radiointercomwireless-audio.html">My Blog: RF24 Wireless Audio</a>
  * @li <a href="https://github.com/TMRh20/RF24Audio/archive/master.zip">Download Current RF24Audio Driver</a>
  * @li <a href="https://github.com/TMRh20/RF24/archive/master.zip">Download Optimized RF24 Radio Driver</a>
  * @li <a href="http://tmrh20.github.com/RF24Network/">Newly Optimized RF24Network Class</a>
  *
  * @section Config Configuration and Setup
  *
  * The settings for the library will generally be detected automatically. To change from the default pin assignments etc, edit the
  * userConfig.h file. The mandatory user options in the configuration file are shown below:
  * @code
  * #define SAMPLE_RATE 24000                     //The sample rate to use for transferring audio samples  Note: 44khz+ sample rate requires 8-bits per sample
  * #define RF_SPEED RF24_1MBPS                   //RF24_250KBPS will do 13-20khz+ sample rate, RF24_1MBPS up to 24-44khz+, RF24_2MBPS for higher. These are not limits, just a guide.
  * #define ANALOG_PIN A0                         //The pin that analog readings will be taken from (microphone pin)
  * @endcode
  *
  * <b>Pin Assignments: See the @link Setup.html @endlink page for wiring diagrams</b>
  * - <b>Speakers:</b> Arduino Uno,Nano,etc: pins 9,10   Arduino Mega: 11,12  (Timer pins cannot be changed, but can use 1 pin and ground)
  * - <b>pin A0:</b> Microphone/Input pin
  * - <b>pin A1:</b> Transmission/Recording Start pin
  * - <b>pin A2:</b> Volume Up
  * - <b>pin A3:</b> Volume Down
  * - <b>pin A4:</b> Trigger remote recording (Only working with dual devices)
  * - <b>Cannot be changed:</b> LED Pin: Uno,Nano,etc: pin 6  Mega 2560: pin 13 (main LED pin)
  *
  *
  * See http://arduino.cc/en/Tutorial/InputPullupSerial for info on how to wire the buttons to the pins
  * See userConfig.h to change default pin assignments and options.
  *
  * @section Advanced Advanced Usage
  *
  * The radio pipes are defined as follows. For more complex multicast scenarios, radios can listen on any combination of pipes. <br>
  * Use the getAddress(); function to access the address array.<br>
  * @code
  * const uint64_t pipes[14] = { 0xABCDABCD71LL, 0x544d52687CLL, 0x544d526832LL, 0x544d52683CLL,0x544d526846LL, 0x544d526850LL,0x544d52685ALL, 0x544d526820LL, 0x544d52686ELL, 0x544d52684BLL, 0x544d526841LL, 0x544d526855LL,0x544d52685FLL,0x544d526869LL};
  * @endcode
  * By default, all radios will open the same reading & writing pipes on the first two addresses.<br>
  * Any radio that wishes to transmit, will reverse the addresses on the first two pipes, and begin to transmit to all the other nodes. <br>
  * Every radio will automatically be assigned the first two addresses, then one of the remaining addresses as a private channel, based on its radio number: <br> (0 = pipes[2], 1 = pipes[3]) <br>
  *
  * Additional addresses can be added by modifying the address array listed in userConfig.h
  *
  * @page Setup Boards & Wiring
  * @section Board Wiring
  * This page displays different options for wiring/board configuration.
  *
  * <img src= "images/NRF1.jpg" height=25% width=25%> <br>
  * Wiring diagram for DIY module connector. May not be needed depending on module:
  * <img src="images/RF24AudioBasic_LargeAntenna.jpg" height="45%" width="45%">
  * <img src= "images/NRF2.jpg" height=20% width=20%>
  * <img src= "images/RF24AudioBasic_SmallAntenna.jpg" height=45% width=45%>
  * Wiring diagram for SD streaming/multicast using TMRpcm library:
  * <img src= "images/RF24Audio_FullSD.jpg" height=65% width=65%>
  *
  */