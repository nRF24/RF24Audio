/**
 * @file userConfig.h
 *
 * macros for customization of RF24Audio
 */

/************  MANDATORY User Variables  ************/
/**
 * @brief The sample rate to use for transferring audio samples
 *
 * Maximum Range: Sample rate 16000, RF_SPEED RF24_250KBPS <br>
 * Maximum Quality: Sample rate 44000, RF_SPEED RF24_2MBPS
 * @note 44khz+ sample rate requires 8-bits per sample
 */
#define SAMPLE_RATE 24000

/**
 * @brief configure the RF data rate
 *
 * These are not limits, just a guide:
 * - RF24_250KBPS will do 13-20khz+ sample rate
 * - RF24_1MBPS up to 24-44khz+
 * - RF24_2MBPS for higher.
 */
#define RF_SPEED RF24_1MBPS

/** @brief The pin that analog readings will be taken from (microphone pin) */
#define ANALOG_PIN A0

/************  OverRides  ************/

//10-bit audio requires more bandwidth. A 20khz sample rate will need 25KB/S transfer rate, which is about max for 250kbps data rate.
//With a 32khz sample rate, the volume can be set to -1 to shift the sample down to 9-bit, which is the highest the timers can handle at 32khz
//#define tenBit       // Enable 10-bit samples   Note: 44khz+ sample rate requires 8-bits per sample

//#define speakerPin 9 // If using a non-standard board, override the timer1 pins
//#define speakerPin 10

/**
 * @brief Indicator pin
 *
 * Using pin 6 on Uno enables audio visualization. Pin 13 on Mega 2560 (TIMER0 COMPA)
 * @note The pin number cannot be changed.
 */
#define ENABLE_LED

/************  Optional/Advanced User Variables  ************/


/** Button pin to trigger recording & transmission */
#define TX_PIN A1

/** Pin for external volume control */
#define VOL_UP_PIN A2

/** Pin for external volume control */
#define VOL_DN_PIN A3

/** Pin for externally triggering remote recording */
#define REMOTE_TX_PIN A4

/** Pin for externally stopping remote recording (needs timeout enabled) */
#define REMOTE_RX_PIN 4

/** The size of the memory buffer to use. Not really configurable (set to maximum by default). */
#define buffSize 32

//#define speakerTX               // Whether to output to speaker while transmitting
//#define oversampling            // Oversampling is recommended for low sample rates only. This only affects playback.
//#define RX_ONLY
//#define TX_ONLY // Not functional yet


/************  Automated pin selections, override by defining above  ************/

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || (__AVR_ATmega32U4__) || (__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__) || (__AVR_ATmega128__) ||defined(__AVR_ATmega1281__)||defined(__AVR_ATmega2561__)
    #define rampMega

    // Speaker pin selection for mega etc
    #if !defined (speakerPin)
        #define speakerPin 11   // The pins to output audio on. (11,12 on Mega 2560)
    #endif

    #if !defined (speakerPin2)
        #define speakerPin2 12
    #endif

    #if defined (ENABLE_LED)
        #define ledPin 13
    #endif

#else

    //Speaker selection for Uno,Nano, etc
    #if !defined (speakerPin)
        /** The pin used to output audio on UNO */
        #define speakerPin 9
    #endif

    #if !defined (speakerPin2)
        /** The pin used to output audio on Nano */
        #define speakerPin2 10
    #endif

    #if defined (ENABLE_LED)
        #define ledPin 6
    #endif
#endif


//********Radio Defines ****************************
/** Radio pipe addresses for the 2 nodes to communicate. */
const uint64_t pipes[14] =
{
    0xABCDABCD71LL,
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
    0x544d526869LL
};
