/* Knock Sensor
  
   Adapted from code here

   http://www.arduino.cc/en/Tutorial/Knock
   
   created 25 Mar 2007
   by David Cuartielles <http://www.0j0.org>
   modified 30 Aug 2011
   by Tom Igoe
   
   This example code is in the public domain.
   
   Basically just prints

 */
 
//didn't use the LED in my setup, but left the code for it


// these constants won't change:
const int ledPin = 13;      // led connected to digital pin 13
const int knockSensor = A0; // the sensor is connected to analog pin 0
const int threshold = 100;  // threshold value to decide when the detected sound is a knock or not


// these variables will change:
int sensorReading = 0;      // variable to store the value read from the sensor pin
int ledState = LOW;         // variable used to store the last LED status, to toggle the light

void setup() {
 pinMode(ledPin, OUTPUT); // declare the ledPin as as OUTPUT
 Serial.begin(9600);       // use the serial port
}

void loop() {
  // read the sensor and store it in the variable sensorReading:
  sensorReading = analogRead(knockSensor);    
  Serial.println("Threshold is:");         
  Serial.println(threshold);     
  // if the sensor reading is greater than the threshold:
  if (sensorReading >= threshold) {
    // toggle the status of the ledPin:
    ledState = !ledState;   
    // update the LED pin itself:        
    digitalWrite(ledPin, ledState);
    // send the string "No motion!" back to the computer, followed by newline
    Serial.println("No motion...reading is");    
    Serial.println(sensorReading);         
    
  }
  else {
    //Send the string "motion" 
  Serial.println("Motion! Reading is");  
  Serial.println(sensorReading);    
  }
  delay(1000);  // delay to avoid overloading the serial port buffer
}
