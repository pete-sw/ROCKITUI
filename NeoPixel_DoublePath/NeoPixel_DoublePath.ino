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

// error timeout of walk

#include <stdint.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define PIN 5
#define PIN2 6
#define N_LEDS 31 // all yellow wires, fast one
#define N_LEDS2 29 // one with colored wires, slow one

Servo myservo1;  // create servo object to control a servo
Servo myservo2;                // a maximum of eight servo objects can be created
Servo myservo3;
Servo myservo4;
int pos1 = 0;    // variable to store the servo position
int pos2 = 90;
int cont = 0;
int done = 0;

// speed
// 0 = off (initial)
// 1 = fast
// 2 = slow
int s = 2;
int i = 0;
int fastdelay = 350;
int slowdelay = 2000;
int timeouts[] = {0,1000,10000}; // how much time you have to step on the next step {fast, slow]}

unsigned long prevMillis=0;
int threshold = 450; // fsr threshold for a step
uint32_t color;

int FSRs[] = {0,1,2,3}; // pins in order for four step sensors.
int fsrvals[] = {0,0,0,0};
int lights[] = {27, 19, 11, 3, 1}; // indices of the lights we will show, in order, for each FSR. add 28
int currentFSR = 0; // index of array
char serInString[40];  // array that will hold the different bytes of the string. 100=100characters;
                        // -> you must state how long the array will be else it won't work properly

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(N_LEDS2, PIN2, NEO_GRB + NEO_KHZ800);

void setup() {
  // attach pins to servos
  myservo1.attach(7);
  myservo2.attach(4);
  myservo3.attach(2);
  myservo4.attach(3);
          
  Serial.begin(9600);
  strip.begin();
  strip.show();
  strip2.begin();
  strip2.show();
  
  /// initial position: closed?
  myservo1.write(pos2);
  myservo2.write(pos1);
  myservo3.write(pos1);
  myservo4.write(pos2);
  delay(3000);
  myservo1.detach();
  myservo2.detach();
  myservo3.detach();
  myservo4.detach();
}
 
void loop() {
  unsigned long currentMillis = millis();

  // clear the string
  memset(serInString, 0, 40);
  //read the serial port and create a string out of what you read
  readSerialString(serInString);
  if(serInString[0] == 'd') {
    // received proper microphone scream
    Serial.println("reset");
    flash(2);

    openDoor(s);
    currentFSR=0;
  }
  if(serInString[0] == 'x') {
    // time out on microphone - never receieved proper audio level
    Serial.println("reset");
    flash(1);

    currentFSR=0;
  }


  // check input -- "active" vs "quiet" or "1,2"
  // if we have time, additional "party" (3) mode where you have to bring a friend.
  if((serInString[0] == 'a' && serInString[1] == 'c') || serInString[0] == '1') {
    // active -- don't want to deal with possible C string manipulation bugs
    s = 1;
  } else if((serInString[0] == 'q' && serInString[1] == 'u') || serInString[0] == '2') {
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

  // if you don't step on the next one in time (fast) we error and reset
  if(done == 0 && currentFSR != 0 && (currentMillis - prevMillis) > timeouts[s]) {
    flash(1);
    currentFSR = 0;
    done = 0;
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
        currentMillis = millis();
        prevMillis = currentMillis;

        if(s == 1 && currentFSR == 3) {
          Serial.println("activecomplete");
          done = 1;
        } else if(s == 2 && currentFSR == 3) {
          Serial.println("quietcomplete");
          done = 1;
        } else {
          currentFSR++;
  
          if(s == 1) {
            // fast delay
            delay(fastdelay);
          } else {
            // slow delay
            delay(slowdelay);
          }
          currentMillis = millis();
          prevMillis = currentMillis;
        }
      } else if(j == currentFSR-1) {
        // still standing on the previous step
      } else {
        // wrong one! error state, and reset!
        // flash all lights red a few times
        flash(1);
  
        // reset current one
        currentFSR = 0;
        done = 0;
      }
    }
  }
}


void flash(int type) {
  uint32_t flashcolor;
  int fdelay = 300;
  if(type == 1) {
    // error
    flashcolor = strip.Color(255, 0, 0);
    fdelay = 300;
  } else if(type == 2) {
    // end success
    flashcolor = strip.Color(0, 0, 255);
    fdelay = 500;
  }
  

  for (int k=0; k < strip.numPixels(); k++) {
    strip.setPixelColor(k, flashcolor);
  }
  for (int k=0; k < strip2.numPixels(); k++) {
    strip2.setPixelColor(k, flashcolor);
  }
  strip.show();
  strip2.show();
  delay(fdelay);
  
  for (int k=0; k < strip.numPixels();k++) {
    strip.setPixelColor(k, 0);
  }
  for (int k=0; k < strip2.numPixels(); k++) {
    strip2.setPixelColor(k, 0);
  }
  strip.show();
  strip2.show();
  delay(fdelay);
  for (int k=0; k < strip.numPixels(); k++) {
    strip.setPixelColor(k, flashcolor);
  }
  for (int k=0; k < strip2.numPixels(); k++) {
    strip2.setPixelColor(k, flashcolor);
  }
  strip.show();
  strip2.show();
  delay(fdelay);
  
  for (int k=0; k < strip.numPixels();k++) {
    strip.setPixelColor(k, 0);
  }
  for (int k=0; k < strip2.numPixels(); k++) {
    strip2.setPixelColor(k, 0);
  }
  strip.show();
  strip2.show();
  delay(fdelay);
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

void openDoor(int type) {
  if(type == 1) {
    // fast
    // open top two servos, 2 and 3
    myservo3.attach(2);
    myservo4.attach(3);
    while (cont <90)
    {
       myservo3.write(cont);
       myservo4.write(pos2-cont);
       cont += 1;
       delay(15);
    }
    delay(15000);
    while(cont>0)
    {
        myservo3.write(cont);
        myservo4.write(pos2-cont);
        cont -= 1;
        delay(15);
    }
    delay(3000);
    myservo3.detach();
    myservo4.detach();
  }  else if(type == 2) {
    // slow
    myservo1.attach(7);
    myservo2.attach(4);

    while (cont <90)
    {
       myservo1.write(pos2-cont);
       myservo2.write(cont);
       cont += 1;
       delay(15);
    }
    delay(15000);
    while(cont>0)
    {
        myservo1.write(pos2-cont);
        myservo2.write(cont);
        cont -= 1;
        delay(15);
    }
    delay(3000);
    myservo1.detach();
    myservo2.detach();
  }
}
