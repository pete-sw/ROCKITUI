// Simple NeoPixel test.  Lights just a few pixels at a time so a
// 1m strip can safely be powered from Arduino 5V pin.  Arduino
// may nonetheless hiccup when LEDs are first connected and not
// accept code.  So upload code first, unplug USB, connect pixels
// to GND FIRST, then +5V and digital pin 6, then re-plug USB.
// A working strip will show a few pixels moving down the line,
// cycling between red, green and blue.  If you get no response,
// might be connected to wrong end of strip (the end wires, if
// any, are no indication -- look instead for the data direction
// arrows printed on the strip).

// two photo cells? maybe?
// groupwork mode?

#include <stdint.h>
#include <Adafruit_NeoPixel.h>
 
#define PIN 5
#define PIN2 6
#define N_LEDS 31 // all yellow wires, fast one
#define N_LEDS2 29 // one with colored wires, slow one

// speed
// 0 = off (initial)
// 1 = fast
// 2 = slow
int s = 0;
int i = 0;
long pm = 0; // previous millis()
int threshold = 450; // fsr threshold for a step
uint32_t color;

int lightPinHi = 3;
int lightPinLo = 4;
int FSRs[] = {0,1,2,3}; // pins in order for four step sensors.
int fsrvals[] = {0,0,0,0};
int lights[] = {2, 10, 18, 26, 28}; // indices of the lights we will show, in order, for each FSR. add 28
int currentFSR = 0; // index of array

char serInString[100];  // array that will hold the different bytes of the string. 100=100characters;
                        // -> you must state how long the array will be else it won't work properly


Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(N_LEDS2, PIN2, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show();
  strip2.begin();
  strip2.show();
}
 
void loop() {
  // clear the string
  memset(serInString, 0, 100);
  //read the serial port and create a string out of what you read
  readSerialString(serInString);

  // check input -- "active" vs "quiet" or "1,2"
  // if we have time, additional "party" (3) mode where you have to bring a friend.
  if((serInString[0] == 'a' && serInString[0] == 'c') || serInString[0] == '1') {
    // active -- don't want to deal with possible C string manipulation bugs
    s = 1;
  } else if((serInString[0] == 'q' && serInString[0] == 'u') || serInString[0] == '2') {
    // quiet
    s = 2;
  }

  // define the color we need
  if(s == 1) {
    // fast
    color = strip.Color(255, 255, 255);
  } else if(s == 2) {
    // slow
    color = strip.Color(100,10,0);
  } else {
    // off
    color = strip.Color(0,0,0);
  }

  // set the color for the correct index only
  for (int j=0; j < strip.numPixels(); j++) {
    if(j == lights[currentFSR] || j == lights[currentFSR]+1) {
      strip.setPixelColor(j, color);
    } else {
      strip.setPixelColor(j, 0);
    }
  }
  for (int j=0; j < strip2.numPixels(); j++) {
    if(j == lights[currentFSR] || j == lights[currentFSR]+1) {
      strip2.setPixelColor(j, color);
    } else {
      strip2.setPixelColor(j, 0);
    }
  }
  strip.show();
  strip2.show();

  // which FSR do we look for next?
  String debug = "LOOKING FOR NUMBER " + String(currentFSR);
  Serial.println(debug);

  // get all the FSR values  
  for(int j = 0 ; j < 4 ; ++j) {
    fsrvals[j] = analogRead(FSRs[j]);
  }

  debug = String(fsrvals[0]) + ", " + String(fsrvals[1]) + ", " + String(fsrvals[2]) + ", " + String(fsrvals[3]);

  // check if any are above threshold
  for(int j=0;j<4;++j) {
    if(fsrvals[j] > threshold) {
      if(j == currentFSR) {
        // you stepped on the right one!
        currentFSR++;
  
        if(s == 1) {
          // fast delay
          delay(1000);
        } else {
          // slow delay
          delay(2000);
        }
      } else if(j == currentFSR-1) {
        // still standing on the previous step
      } else {
        // wrong one! error state, and reset!
        // flash all lights red a few times
        for (int k=0; k < strip.numPixels(); k++) {
          strip.setPixelColor(k, 255,0,0);
        }
        for (int k=0; k < strip2.numPixels(); k++) {
          strip2.setPixelColor(k, 255,0,0);
        }
        strip.show();
        strip2.show();
        delay(300);
  
        for (int k=0; k < strip.numPixels();k++) {
          strip.setPixelColor(k, 0);
        }
        for (int k=0; k < strip2.numPixels(); k++) {
          strip2.setPixelColor(k, 0);
        }
        strip.show();
        strip2.show();
        delay(300);
        for (int k=0; k < strip.numPixels(); k++) {
          strip.setPixelColor(k, 255,0,0);
        }
        for (int k=0; k < strip2.numPixels(); k++) {
          strip2.setPixelColor(k, 255,0,0);
        }
        strip.show();
        strip2.show();
        delay(300);
  
        for (int k=0; k < strip.numPixels();k++) {
          strip.setPixelColor(k, 0);
        }
        for (int k=0; k < strip2.numPixels(); k++) {
          strip2.setPixelColor(k, 0);
        }
        strip.show();
        strip2.show();
        delay(300);
  
  
        // reset current one
        currentFSR = 0;
      }
    }
  }
}

//read a string from the serial and store it in an array
//you must supply the array variable
void readSerialString (char *strArray) {
  int i = 0;
  if(!Serial.available()) {
    return;
  }
  while (Serial.available()) {
    strArray[i] = Serial.read();
    i++;
  }
}

