# RF24Audio - Realtime Audio Streaming Library for Arduino

@tableofcontents

This class implements a realtime audio streaming solution using nRF24L01(+) radios driven
by the newly [optimized RF24 library fork](http://nRF24.github.com/RF24/).

## Features
- <b>User friendly setup and configuration:</b> For beginners too: Just connect a radio module, microphone, and speaker. The library handles the rest.
- <b>Recording and broadcasting of audio</b> to multiple devices using only Arduino, RF24 modules and input/output (speaker/microphone) devices
- <b>Multicast:</b> Enables broadcasting to all nodes, single nodes, or partial groups of nodes
- <b>External controls:</b> Use external buttons or code to control audio streaming
- <b>Volume control:</b> Use external buttons or code to control audio volume on receiving devices.
- <b>Remote control:</b> Start recording remotely via radio commands (Currently cannot stop remote recording)
- <b>LED Indicator/Visualization:</b> Indicates audio playback and amplitude.
- <b>Customization:</b> Using the underlying RF24 core library allows custom interaction with audio devices running this library. Receive audio data
  and stream it to a PC over USB, create and broadcast computer generated audio in realtime, and more! See the [USB_Audio example](USB_Audio_8ino-example.html) for more info.
- <b>Create additional node groups:</b> Allows nodes to join private broadcast groups, and multicast only within their group as desired. See advanced section below.

## How to learn more
- [RF24Audio Library Class Documentation](classRF24Audio.html)
- [RF24: Underlying radio driver (2014 - Newly Optimized)](http://nRF24.github.io/RF24/)
- [My Blog: RF24 Optimization Overview](http://tmrh20.blogspot.com/2014/03/high-speed-data-transfers-and-wireless.html)
- [My Blog: RF24 Wireless Audio](http://tmrh20.blogspot.com/2014/03/arduino-radiointercomwireless-audio.html)
- [Download Current RF24Audio Driver](https://github.com/TMRh20/RF24Audio/archive/master.zip)
- [Download Optimized RF24 Radio Driver](https://github.com/TMRh20/RF24/archive/master.zip)
- [Newly Optimized RF24Network Class](http://nRF24.github.io/RF24Network/)

## Configuration and Setup
The settings for the library will generally be detected automatically. To change from the default pin assignments etc, edit the
userConfig.h file. The mandatory user options in the configuration file are shown below:

```cpp
#define SAMPLE_RATE 24000   // The sample rate to use for transferring audio samples  Note: 44khz+ sample rate requires 8-bits per sample
#define RF_SPEED RF24_1MBPS // RF24_250KBPS will do 13-20khz+ sample rate, RF24_1MBPS up to 24-44khz+, RF24_2MBPS for higher. These are not limits, just a guide.
#define ANALOG_PIN A0       // The pin that analog readings will be taken from (microphone pin)
```

### Pin Assignments
@see [the Setup page](Setup.html) for wiring diagrams

- <b>Speakers:</b> Arduino Uno,Nano,etc: pins 9, 10   Arduino Mega: 11, 12  (Timer pins cannot be changed, but can use 1 pin and ground)
- <b>pin A0:</b> Microphone/Input pin
- <b>pin A1:</b> Transmission/Recording Start pin
- <b>pin A2:</b> Volume Up
- <b>pin A3:</b> Volume Down
- <b>pin A4:</b> Trigger remote recording (Only working with dual devices)
- <b>Cannot be changed:</b> LED Pin: Uno,Nano,etc: pin 6  Mega 2560: pin 13 (main LED pin)

See http://arduino.cc/en/Tutorial/InputPullupSerial for info on how to wire the buttons to the pins
See userConfig.h to change default pin assignments and options.

## Advanced Usage
The radio pipes are defined as follows. For more complex multicast scenarios, radios can listen on any combination of pipes. <br>
Use the getAddress(); function to access the address array.

```cpp
const uint64_t pipes[14] = {0xABCDABCD71LL,
                            0x544d52687CLL,
                            0x544d526832LL,
                            0x544d52683CLL,
                            0x544d526846LL,
                            0x544d526850LL,
                            0x544d52685ALL,
                            0x544d526820LL,
                            0x544d52686ELL,
                            0x544d52684BLL,
                            0x544d526841LL,
                            0x544d526855LL,
                            0x544d52685FLL,
                            0x544d526869LL};
```
By default, all radios will open the same reading & writing pipes on the first two addresses.<br>
Any radio that wishes to transmit, will reverse the addresses on the first two pipes, and begin to transmit to all the other nodes. <br>
Every radio will automatically be assigned the first two addresses, then one of the remaining addresses as a private channel, based on its radio number: <br> (0 = pipes[2], 1 = pipes[3]) <br>

Additional addresses can be added by modifying the address array listed in userConfig.h
