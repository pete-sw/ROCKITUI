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
 
#include <Adafruit_NeoPixel.h>
 
#define PIN 5
#define PIN2 6
#define N_LEDS 31 // all yellow wires, fast one
#define N_LEDS2 29 // one with colored wires, slow one
// speed. 1 = fast
// 2 = slow
int s = 2;
int i = 0;
long pm = 0; // previous millis()
int fsrval = 0;
int threshold = 500; // fsr threshold for a step

int fastFSRs[] = {0,1,2}; // pins in order
int slowFSRs[] = {3,4,5};
int currentFSR = 0; // index of array

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
  // which FSR do we look for next?
  String debug = "LOOKING FOR NUMBER " + String(currentFSR);
  Serial.println(debug);
  if(s == 1) {
    fsrval = analogRead(fastFSRs[currentFSR]);
  } else {
    fsrval = analogRead(slowFSRs[currentFSR]);
  }
  Serial.println(fsrval);
  if(fsrval > threshold) {
    currentFSR++;
    if(currentFSR == 2) {
      Serial.println("ALL THREE STEPS DONE");
    }
  }
  
  if(s == 1) {
    // fast
    chase(1, strip.Color(255, 255, 255), 50);
  } else {
    // slow
    chase(2, strip.Color(100, 10, 0), 125);
  }
}
 
static void chase(int stripNum, uint32_t c, long wait) {
  unsigned long currentMillis = millis();

  int nleds;
  if (stripNum == 1) {
    nleds = N_LEDS;
  } else {
    nleds = N_LEDS2;
  }

  if(currentMillis - pm > wait) {
    // save the last time we changed the lights.
    pm = currentMillis;
    if(i<nleds+4) {
      if(stripNum == 1) {
        strip.setPixelColor(i , c); // Draw new pixel
        strip.setPixelColor(i-4, 0); // Erase pixel a few steps back
        strip.show();
      } else {
        strip2.setPixelColor(i , c); // Draw new pixel
        strip2.setPixelColor(i-4, 0); // Erase pixel a few steps back
        strip2.show();
      }
      ++i;
    } else {
      i = 0;
      if(stripNum == 1) {
        strip.setPixelColor(i , c); // Draw new pixel
        strip.setPixelColor(i-4, 0); // Erase pixel a few steps back
        strip.show();
      } else {
        strip2.setPixelColor(i , c); // Draw new pixel
        strip2.setPixelColor(i-4, 0); // Erase pixel a few steps back
        strip2.show();
      }
    }
  }
//  for(uint16_t i=0; i<nleds+4; i++) {
//    if(stripNum == 1) {
//      strip.setPixelColor(i , c); // Draw new pixel
//      strip.setPixelColor(i-4, 0); // Erase pixel a few steps back
//      strip.show();
//      delay(wait);
//    } else {
//      strip2.setPixelColor(i , c); // Draw new pixel
//      strip2.setPixelColor(i-4, 0); // Erase pixel a few steps back
//      strip2.show();
//      delay(wait);
//    }      
//  }
}
