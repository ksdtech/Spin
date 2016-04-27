/*
  Spin
  Default sketch for controlling the Spin turtntable using the Spin iOS app
  Also includes debugging functionality to use with the Processing Sketch
  Find additional documentation at spin.media.mit.edu/build
  Stepper Motor Code Modified from BILDR: http://bildr.org/2011/06/easydriver/
  
  Author: Tiffany Tseng  
  Last Updated: 10/19/2015
  Changed motor_speed for faster spins
  
*/

#include <Stepper.h>
#include <SoftModem.h>
#include <ctype.h>

#define DIR_PIN 2
#define STEP_PIN 9

#define LED_PIN 13
#define LED_PIN_1 A2
#define LED_PIN_2 A3
#define LED_PIN_3 A4
#define LED_PIN_4 A5

#define PHONE_PIN 4

// Uncomment to test stepper motor and driver
// #define DEBUG_MOTOR

SoftModem modem;

boolean rotating = false;
boolean cancelled = false;
int numRotations = 15; // should be 15
int currentRotation = 0;
char val;
float rotAmt = 240;
float motor_speed = 0.2;

void setup() { 
  pinMode(DIR_PIN, OUTPUT); 
  pinMode(STEP_PIN, OUTPUT); 
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  pinMode(PHONE_PIN, INPUT);
  Serial.begin(9600);
  modem.begin();

#if defined(DEBUG_MOTOR)
  debugMotor();
#endif
} 

void loop() {

#if defined(DEBUG_MOTOR)
    return;
#else  

  if (!rotating) {
    digitalWrite(LED_PIN_3, HIGH);
    digitalWrite(LED_PIN_1, HIGH);
  }

  if (modem.available()) {
    // RECEIVED SIGNAL FROM IPHONE 
    int input = modem.read();
    Serial.print(input);

    if (input == 253) {
      // begin spin - flash LEDs
      Serial.print(" start ");
      modem.write(0xFC); // send confirmation signal to app
      flashLEDsOnce();
      currentRotation = 0;
      rotating = true;
      cancelled = false;
      delay(1000);
    } else if (input == 251) {
      // stop arduino
      rotating = false;
      cancelled = true;
      currentRotation = 0;
      Serial.print(" stop - writing stop signal");
      modem.write(0xFE);
    } else if (input == 234) {
      // setup start received
      flashLEDsOnce();
      modem.write(0xEC);
      rotateDeg(-rotAmt/2, motor_speed);
      rotateDeg(rotAmt/2, motor_speed);
    }
  }
  
  if (rotating && !cancelled && currentRotation < numRotations) {
    // Serial.println(currentRotation+1);
    singleRotation();
    currentRotation++;
  } else {
    rotating = false;
    cancelled = true;
  }
  
  if (Serial.available()) {
    // SIGNAL RECEIVED FROM SERIAL MONITOR / PROCESSING
    val = Serial.read();
    // read serial character from the serial monitor and send to arduino  
    // blink LED to show that signal was sent to iPhone
    ledsOn();
    delay(2);
    ledsOff();

    // KEYBOARD INPUT FOR DEBUGGING THROUGH PROCESSING
    if (val == 'L') {
      // nudge the motor CW 
      ledsOn();
      rotateDeg(-rotAmt, motor_speed);
      ledsOff();
    } else if (val == 'R') {
      // nudge the motor CCW 
      ledsOn();
      rotateDeg(rotAmt, motor_speed);
      ledsOff();    
    } else if (val == 'S') {
      // do a full 360 spin, stopping 15 times
      takePhoto();
      for (int i = 0; i < numRotations; i++) {
        ledsOn();
        // move the motor
        rotateDeg(-rotAmt, motor_speed);
        ledsOff();
        if (i != numRotations-1) {
          takePhoto();
        }
      }
      flashLEDs();
    } else if (val == 'T') {
      // do a full 360 spin, no stopping
      ledsOn();
      rotateDeg(-3600.0, motor_speed);
      ledsOff();
    } else if (val == 'P') {
      // take a photo
      takePhoto();
    }
  }

#endif
}

void singleRotation() {
  rotating = true;
  ledsOn();
  // Serial.println("motor turning");
  rotateDeg(-rotAmt, motor_speed);
  // Serial.println("motor stopped");
  ledsOff();
  takePhoto();
}

void takePhoto() {
  // snap a photo
  // Serial.println("start delay");
  // time it takes to capture photo and add imageview to app
  delay(1950);
  // Serial.println("end delay");
}

void rotate(int steps, float speed) { 
  // rotate a specific number of microsteps (8 microsteps per step) - (negative for reverse movement)
  // speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
  int dir = (steps > 0) ? HIGH : LOW;
  steps = abs(steps);

  digitalWrite(DIR_PIN,dir); 

  float usDelay = (1/speed) * 70;

  for (int i = 0; i < steps; i++) { 
    digitalWrite(STEP_PIN, HIGH); 
    delayMicroseconds(usDelay); 

    digitalWrite(STEP_PIN, LOW); 
    delayMicroseconds(usDelay); 
  } 
} 

void rotateDeg(float deg, float speed) { 
  // rotate a specific number of degrees (negitive for reverse movement)
  // speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
  int dir = (deg > 0) ? HIGH : LOW;
  digitalWrite(DIR_PIN,dir); 

  int steps = abs(deg)*(1/0.225);
  float usDelay = (1/speed) * 70;

#if defined(DEBUG_MOTOR)
  Serial.print("rotateDeg ");
  Serial.print(deg);
  Serial.print(" degrees, speed ");
  Serial.println(speed);
  Serial.print(" with DIR ");
  Serial.print(dir);
  Serial.print(", in ");
  Serial.print(steps);
  Serial.print(" steps, usDelay ");
  Serial.println(usDelay);
#endif

  for (int i = 0; i < steps; i++) { 
    digitalWrite(STEP_PIN, HIGH); 
    delayMicroseconds(usDelay); 

    digitalWrite(STEP_PIN, LOW); 
    delayMicroseconds(usDelay); 
  }

#if defined(DEBUG_MOTOR)
  Serial.println("rotateDeg done"); 
#endif

}

#if defined(DEBUG_MOTOR)
void debugMotor() {
  ledsOn();
  delay(2);
  ledsOff();
  delay(2);
  ledsOn();
  rotateDeg(-rotAmt, motor_speed);
  ledsOff();
}
#endif

void ledsOn() {
  digitalWrite(LED_PIN_1, HIGH);
  digitalWrite(LED_PIN_2, HIGH);
  digitalWrite(LED_PIN_3, HIGH);
  digitalWrite(LED_PIN_4, HIGH);
}

void ledsOff() {
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  digitalWrite(LED_PIN_4, LOW);
}  

void flashLEDs() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN_1, HIGH);
    digitalWrite(LED_PIN_2, HIGH);
    digitalWrite(LED_PIN_3, HIGH);
    digitalWrite(LED_PIN_4, HIGH);
    delay(300);
    digitalWrite(LED_PIN_1, LOW);
    digitalWrite(LED_PIN_2, LOW);
    digitalWrite(LED_PIN_3, LOW);
    digitalWrite(LED_PIN_4, LOW);
    delay(300);
  }  
}

void flashLEDsOnce() {
  digitalWrite(LED_PIN_1, HIGH);
  digitalWrite(LED_PIN_2, HIGH);
  digitalWrite(LED_PIN_3, HIGH);
  digitalWrite(LED_PIN_4, HIGH);
  delay(100);
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  digitalWrite(LED_PIN_4, LOW);
}
