/*
   Rampart Bytebeats @copyright 2024, Mark Washeim <blueprint@poetaster.de>
   GPLv3, see LICENSE
   some parts use other licensces (nyblybyte.h is MIT)
   and some parts are Public Domain in as far as permissible by law

   a number of formulas https://raw.githubusercontent.com/schollz/nyblcore/main/bytebeat/bytebeat.ino

   Many contributions from the internet :) See nyblybyte.h for many equations origins and original form.
*/

#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include <hardware/pwm.h>
#include <Wire.h>

#include <PWMAudio.h>
#define SAMPLERATE 8000
#define PWMOUT 11

PWMAudio DAC(PWMOUT);  // 16 bit PWM audio
float global_volume = 0.8;

// display related
const int oled_sda_pin = 2;
const int oled_scl_pin = 3;
const int oled_i2c_addr = 0x3C;
const int dw = 128;
const int dh = 32;

// TOF on same bus as adafruit
//#include <VL53L1X.h>
//VL53L1X sensor;

// the adc 4 inputs 16 bits
#include <AlashADS1115.h>
AlashADS1115 adc;


#include <Adafruit_SSD1306.h>
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(dw, dh, &Wire1, OLED_RESET);
#include "font.h"
#include "helvnCB6pt7b.h"
#define myfont helvnCB6pt7b // Org_01 looks better but is small.
#include "display.h"

#include "bytebeats.h"
const unsigned int TOP = 0x07FF; // 11-bit resolution.  7812 Hz PWM

// noise reduction for cv reads
uint16_t potvalue[3]; // pot readings
uint16_t lastpotvalue[3]; // old pot readings
#define POT_SAMPLE_TIME 30 // delay time between pot reads
#define MIN_POT_CHANGE 15 // locked pot reading must change by this in order to register
#define MIN_COUNTS 5  // unlocked pot must change by this in order to register
#define POT_AVERAGING 3 // analog sample averaging count 
#define POT_MIN 4   // A/D may not read min value of 0 so use a bit larger value for map() function
#define POT_MAX 1023 // A/D may not read max value of 1023 so use a bit smaller value for map() function

//#define LEDPIN   13 // usually 13

// for the sine waves
uint16_t Acc[3];
unsigned long freq[3];
unsigned int tune, pitch;
byte pot[] = {29, 28, 27};
// from levitation osc.


volatile long t = 0;
volatile int a, b, c, i;
volatile int offA, offB, offC;
volatile int result;
int d = 0; // hmm?

volatile int currentBank = 3;
volatile Bank banks[4] = {
  {1, 26},
  {1, 29},
  {1, 20},
  {1, 30}
};

int numProg = 73;

// these ranges are provisional and in schollz equations need to be reset
volatile int aMax = 99, aMin = 0, bMax = 99, bMin = 0, cMax = 99, cMin = 0;

// default rate close to the original bytebeat speed
volatile int SRATE = 1128; 

bool debug = true;

// encoder
#include <RotaryEncoder.h>
const int encoderA_pin = 10;
const int encoderB_pin = 9;

RotaryEncoder encoder(encoderB_pin, encoderA_pin, RotaryEncoder::LatchMode::FOUR3);

void checkEncoderPosition() {
 encoder.tick();   // call tick() to check the state.
}

#include "Switch.h"
// button inputs
#define BUTTONL  5 // these are the touch surfaces
#define BUTTONR 6 
#define BUTTONE 8 
Switch  buttonL(5, 3, false);
Switch  buttonR(6, 3, false);
Switch  buttonE(8, 3, false);

int encoder_pos_last = 0;
long encoder_delta = 0;
volatile int enc_offset = 1; // changes direction
int enc_delta; // which direction
uint32_t encoder_push_millis;
uint32_t step_push_millis;
bool encoder_held = false;


struct repeating_timer bytebeats_timer;
//counter for sound
float floatI = 0;
float tempo = 16;
int currentSound = 4;
int iterations = 256;

// analog freq pins OLD
#define TEMPO 0
#define ITERATIONS 1

long timeoffset = 0;

/**
    handle left button short release
*/
void onLeftReleased() {
  if (debug) {
    Serial.println("onLeftReleased TRIGGERED");
  }
  if (banks[currentBank].currentProg > 1) {
    banks[currentBank].currentProg--;
  } else {
    banks[currentBank].currentProg = banks[currentBank].maxProg;
  }

  if (debug) {
    Serial.print("PROGRAM: ");
    Serial.println(banks[currentBank].currentProg);
  }
}

/**
    handle right button short release
*/
void onRightReleased() {
  if (debug) {
    Serial.println("onRightReleased TRIGGERED");
  }
  if (banks[currentBank].currentProg < banks[currentBank].maxProg) {
    banks[currentBank].currentProg++;
  } else {
    banks[currentBank].currentProg = 1;
  }

  if (debug) {
    Serial.print("PROGRAM: ");
    Serial.println(banks[currentBank].currentProg);
  }
}

// values to compare input on CV pins
int lastA = 0;
int lastB = 0;
int lastC = 0;

void setup() {

  if (debug) {
    Serial.begin(57600);
    Serial.println(F("Started"));
  }

  // Additions
  // ENCODER
  pinMode(encoderA_pin, INPUT_PULLUP);
  pinMode(encoderB_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderA_pin), checkEncoderPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderB_pin), checkEncoderPosition, CHANGE);

  // DISPLAY and TOF are on this bus
  Wire1.setSDA(oled_sda_pin);
  Wire1.setSCL(oled_scl_pin);
  Wire1.begin();
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.display();
  displaySplash();

 // this is the ADCs I2C
  Wire.setSDA(12);
  Wire.setSCL(13);
  Wire.setClock(100000); // use 100 kHz I2Ca
  Wire.begin();
  // start
  adc.begin();
  adc.setRate(860);
  adc.setAutoGain(true);

  // let's get more resolution
  //analogReadResolution(12);
  pinMode(26, INPUT);  //pin block
  //pinMode(LEDPIN, OUTPUT);
  //pinMode(PWMPIN, OUTPUT);

  pinMode(8, INPUT_PULLUP);  //pin block
  lastA =  0; //analogRead(A3);
  lastB =  0; //analogRead(A6);
  lastC =  0; //analogRead(A7);

  // set up Pico PWM audio output
  DAC.setBuffers(4, 32);  // DMA buffers
  DAC.setFrequency(SAMPLERATE);
  DAC.begin();

}


void updateControl() {

  uint32_t now = millis();
  bool anybuttonpressed;
  anybuttonpressed = false;

  byte pinState; // commone state for button check

  if (millis() > timeoffset + 150 ) {
    pinState = buttonL.stateDebounced();
    // button reads for bank changes
    if (buttonL.changed() && pinState == 0 ) {
        onLeftReleased();
    }

    pinState = buttonR.stateDebounced();
    // button reads for program changes
    if (buttonR.changed() && pinState == 0 ) {
        onRightReleased();
    }

    pinState = buttonE.stateDebounced();
    if (buttonE.changed() && pinState == 0 ) {
     encoder_push_millis = now;
     currentBank = currentBank + 1;
     if (currentBank > 3) currentBank = 0;
     if (debug) {
       Serial.print("bank: ");
       Serial.println(currentBank );
     }
    } else {
      encoder_push_millis = 0;
      encoder_held = false;
    }
  }
  // first encoder
  encoder.tick();

  int encoder_pos = encoder.getPosition();
  if ( (encoder_pos != encoder_pos_last )) {
    encoder_delta = encoder_pos - encoder_pos_last;
  }

  if ( encoder_pos != encoder_pos_last  ) {
    long cstep = (long)encoder_delta * 32; 
    SRATE = constrain(SRATE + cstep, 1000, 48000);
      if (debug) {
        Serial.print("SRATE changing: ");
        Serial.print(oSRATE);
        Serial.print(" -> ");
        Serial.println(SRATE);
      }
  }

  // now, after buttons check if only encoder moved and no buttons
  if (! anybuttonpressed && encoder_delta) {
    //float turn = encoder_delta * 0.01f;
    //harm_in = harm_in + turn;

  }
  // don't call adc to often
  if (millis() > timeoffset + 25) {
    readAdc();
  }

  /// only set new pos last after buttons have had a chance to use the delta
  encoder_pos_last = encoder_pos;
  encoder_delta = 0;  // we've used it

  // start tracking time encoder button held
  // EncoderButton object updates
  if (millis() > timeoffset + 150 ) {
    //if (debug) Serial.println("hit timer offset");
    timeoffset = millis();
    //display_value(SRATE);
    //if (result > 5000) digitalWrite(LEDPIN, HIGH);
    //if (result < 1000) digitalWrite(LEDPIN, LOW);

  }

  // refresh the scope: samples of the running bytebeat function
  static uint32_t last_scope = 0;
  if (now - last_scope > 50) { // every 50 ms
    last_scope = now;
    drawScope();
  }
}

void loop() {

  DAC.setFrequency(SRATE);
  if ( DAC.availableForWrite()) {
      bytebeats_process();
      DAC.write(result);
      scope_push(result); // record the rendered sample for the scope
    }
}

void setup1() {
  // Core 1 setup if needed.
}

void loop1() {
  updateControl(); // required here
  knobs();
}


// this is just very wrong ;)
// take the average on the input on pin 3,6,7
void readAdc() {

    //uint16_t A =  map(analogRead(26), 0, 1023, aMin, aMax);
    float A =  adc.voltage(0);
    A =  map(A, 0.0, 3.0, aMin, aMax);
    float B =  map(adc.voltage(1), 0.0, 3.0, bMin, bMax);
    float C =  map(adc.voltage(2), 0.0, 3.0, cMin, cMax);

      if (abs(lastA - A) > 1) {
        lastA = A;
        offA = lastA - 1;
      }

      if (abs(lastB - B) > 1) {
        lastB = B;
        offB = lastB - 1;
      }

      if (abs(lastC - C) > 1) {
        lastC = C;
        offC = lastC - 1 ;
      }
}

uint16_t readcv(uint8_t potnum) {
  int val = 0;
  int input = pot[potnum];
  for (int j = 0; j < POT_AVERAGING; ++j) val += (analogRead(input)); // read the A/D a few times and average for a more stable value
  val = val / POT_AVERAGING;
 
  if (abs(lastpotvalue[potnum] - val) > MIN_COUNTS ) { // even if pot is unlocked, make sure pot has moved by at least MIN_COUNT counts so values don't jump around
    lastpotvalue[potnum] = val; // pot is unlocked, so save the reading
  }  else {
    val = lastpotvalue[potnum];
  }

 potvalue[potnum] = val; // pot is unlocked so save the reading

  return val;
}

// pot inputs
void knobs() {
  // includes offset from 3 cv inputs
  a = map(analogRead(29), 0, 1023, aMin, aMax) + offA;
  constrain(a, aMin, aMax);
  b = map(analogRead(28), 0, 1023, bMin, bMax) + offB ;
  constrain(b, bMin, bMax);
  c = map(analogRead(27), 0, 1023, cMin, cMax) + offC;
  constrain(c, cMin, cMax);
/*  if (debug){
    Serial.print("a ");
    Serial.print(a);
    Serial.print(" b ");
    Serial.print(b);
    Serial.print(" c ");
    Serial.println(c);
  }*/
}


// show a number in binary on the LEDs
void display_value(int16_t value) {
  for (int i = 7; i >= 0; i--) { // NOPE + 1 can loop this way since port assignments are sequential
    //digitalWrite(led[i], result & 1);
    value = value >> 1;
  }
  //display_timer = millis();
}
