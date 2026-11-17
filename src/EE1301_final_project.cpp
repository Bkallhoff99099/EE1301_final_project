/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "neopixel.h"
#include <string>

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

int redButtonPin = D1;
int blueButtonPin = D2;
int greenButtonPin = D3;
int yellowButtonPin = D4;
int servoPin = A5; //must be either A5 or A2
int speakerPin = A2; //must be either A5 or A2

Servo servo; //do not initialize in the setup method
const int servoClosed = 90;
const int servoOpen = 0;

int pixel_count = 3;
#define PIXEL_PIN SPI
int PIXEL_TYPE = WS2812;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixel_count, PIXEL_PIN, PIXEL_TYPE);

enum programState {
  LOCKED,
  OPEN,
  ALARM,
  SET_PASSWORD //unsure if this is needed
};

//cloud variables
programState currState = LOCKED;
string currPassword = "rgbrgb"; 
string currGuess = "......";
int currGuesIndex = 0;

bool verifyGuess(int guess); //checks if currentGuess matches the password
void changeState(programState newState); //allows for smooth transitions to start/end states
void modifyGuess(char newChar); //used to add input to current guess

//cloud methods
void inputHandler(int buttonID); //takes in buttonID and executes the corresponding action


// setup() runs once, when the device is first turned on
void setup() {
  pinMode(redButtonPin, INPUT_PULLDOWN);
  pinMode(blueButtonPin, INPUT_PULLDOWN);
  pinMode(greenButtonPin, INPUT_PULLDOWN);
  pinMode(yellowButtonPin, INPUT_PULLDOWN);
  strip.begin();
  // Put initialization like pinMode and begin functions here
}

void inputHandler(int buttonID){
  switch(input){
    case 0: //red button
      modifyGuess('r');
      break;
    case 1: //green button
      modifyGuess('g');
      break;
    case 2: //blue button
      modifyGuess('b');
      break;
    case 3: //yellow/lock button
      changeState(LOCKED);
      break;
    case 4: //reset password button
      changeState(SET_PASSWORD);
      break;
  }
}

void changeState(programState newState){
  if(newState == LOCKED){
    servo.attach(servoPin);
    servo.write(servoClosed);
    servo.detach(); //necessary to use speaker
  }else if(newState == OPEN){
    servo.attach(servoPin);
    servo.write(servoOpen);
    servo.detach(); //necessary to use speaker
  }else if(newState == ALARM){

  }else if(newState == SET_PASSWORD){

  }
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

  // Example: Publish event to cloud every 10 seconds. Uncomment the next 3 lines to try it!
  // Log.info("Sending Hello World to the cloud!");
  // Particle.publish("Hello world!");
  // delay( 10 * 1000 ); // milliseconds and blocking - see docs for more info!
}
