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
using namespace std;

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

int redButtonPin = D2;
int blueButtonPin = D4;
int greenButtonPin = D3;
int yellowButtonPin = D5;
int servoPin = A5; //must be either A5 or A2
int speakerPin = A2; //must be either A5 or A2

const int RED_BUTTON_ID = 0;
const int BLUE_BUTTON_ID = 1;
const int GREEN_BUTTON_ID = 2;
const int YELLOW_BUTTON_ID = 3;
const int RESET_PASSWORD_BUTTON_ID = 4;
const int RESET_ALARM_BUTTON_ID = 5;

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
  SET_PASSWORD 
};


programState currState = LOCKED;
string currPassword = "rgbrgb"; 
string currGuess = "......";
int currGuessIndex = 0;
int num_failed_attempts = 0;

bool verifyGuess(string guess); //checks if currentGuess matches the password
void changeState(programState newState); //allows for smooth transitions to start/end states
void modifyGuess(char newChar); //used to add input to current guess
void resetAlarm();
void handleLEDs();
void inputHandler(int buttonID); //takes in buttonID and executes the corresponding action

//cloud methods
int cloud_input_handler(string input);

//cloud variables
bool password_verified = false;

// setup() runs once, when the device is first turned on
void setup() {
  pinMode(redButtonPin, INPUT_PULLDOWN);
  pinMode(blueButtonPin, INPUT_PULLDOWN);
  pinMode(greenButtonPin, INPUT_PULLDOWN);
  pinMode(yellowButtonPin, INPUT_PULLDOWN);
  Particle.variable("User_verified_password", password_verified);
  Particle.function("cloud_input", cloud_input_handler);
  strip.begin();
  // Put initialization like pinMode and begin functions here
}

int cloud_input_handler(string input){
  int inputID = stoi(input);
  inputHandler(inputID);
}

void inputHandler(int buttonID){
  switch(buttonID){
    case RED_BUTTON_ID: 
      if(currState == LOCKED || currState == SET_PASSWORD){
        modifyGuess('r');
      }
      break;
    case GREEN_BUTTON_ID:
      if(currState == LOCKED || currState == SET_PASSWORD){
        modifyGuess('g');
      }
      break;
    case BLUE_BUTTON_ID: //blue button
      if(currState == LOCKED || currState == SET_PASSWORD){
        modifyGuess('b');
      }
      break;
    case YELLOW_BUTTON_ID: //yellow/lock button
      if(currState == OPEN){
        changeState(LOCKED);
      }
      break;
    case RESET_PASSWORD_BUTTON_ID: //reset password button
      changeState(SET_PASSWORD);
      break;
    case RESET_ALARM_BUTTON_ID:
      resetAlarm();
      break;
  }
}

void changeState(programState newState){
  if(newState == LOCKED){
    password_verified = false;
    servo.attach(servoPin);
    servo.write(servoClosed);
    servo.detach(); //necessary to use speaker
  }else if(newState == OPEN){
    password_verified = true;
    num_failed_attempts = 0;
    servo.attach(servoPin);
    servo.write(servoOpen);
    servo.detach(); //necessary to use speaker
  }else if(newState == ALARM){
    Particle.publish("Alarm_triggered");
  }else if(newState == SET_PASSWORD){
    currGuess = "......";

  }
}

bool verifyGuess(string guess){
  for(int i = 0; i < 6; i++){
    if(guess[i] != currPassword[i])
        return false;
  }
  return true; //only runs if all match
}

void modifyGuess(char newChar){
  currGuess[currGuessIndex] = newChar;
  currGuessIndex++;
  if(currGuessIndex >= 6){

    if(currState == LOCKED){ //normal behavior
      if(verifyGuess(currGuess)){
        changeState(OPEN);
      }else{
        num_failed_attempts++;
        if(num_failed_attempts > 3){
          changeState(ALARM);
        }
      }

    }else if (currState == SET_PASSWORD){ //reset password behavior
      if(password_verified){
        currPassword = currGuess;
      }else{
        password_verified = true;
      }
    }

    currGuessIndex = 0;
    currGuess = "......";
    
  }
}

void resetAlarm(){
  if(currState == ALARM){ //prevents unintended state changes
    num_failed_attempts = 0;
    changeState(LOCKED);
  }
}

void handleLEDs(){
  switch(currState){
    case LOCKED:
      for(int i = 0; i < num_failed_attempts; i++){
        strip.setPixelColor(i, strip.Color(0, 255, 0)); //sets to red
      }
      break;
    case OPEN:
      for(int i = 0; i < pixel_count; i++){
        strip.setPixelColor(i, strip.Color(255,0,0)); //sets to green
      }
      break;

    default:
      break;
  }

  strip.show();
}

#define PRESSED true;
#define NOT_PRESSED false;
bool prevRedButtonState = NOT_PRESSED;
bool prevBlueButtonState = NOT_PRESSED;
bool prevGreenButtonState = NOT_PRESSED;
bool prevYellowButtonState = NOT_PRESSED;

// loop() runs over and over again, as quickly as it can execute.
void loop() {

  //physical red button handling
  if(digitalRead(redButtonPin) == HIGH && !prevRedButtonState){
    inputHandler(RED_BUTTON_ID);
    prevRedButtonState = PRESSED;
  }
  if(digitalRead(redButtonPin) == LOW){
    prevRedButtonState = NOT_PRESSED;
  }

  //physical blue button handling
  if(digitalRead(blueButtonPin) == HIGH && !prevBlueButtonState){
    inputHandler(BLUE_BUTTON_ID);
    prevBlueButtonState = PRESSED;
  }
  if(digitalRead(blueButtonPin) == LOW){
    prevBlueButtonState = NOT_PRESSED;
  }

  //physical green button handling
  if(digitalRead(greenButtonPin) == HIGH && !prevGreenButtonState){
    inputHandler(GREEN_BUTTON_ID);
    prevGreenButtonState = PRESSED;
  }
  if(digitalRead(greenButtonPin) == LOW){
    prevGreenButtonState = NOT_PRESSED;
  }


  //physical yellow button handling
  if(digitalRead(yellowButtonPin) == HIGH && !prevYellowButtonState){
    inputHandler(YELLOW_BUTTON_ID);
    prevYellowButtonState = PRESSED;
  }
  if(digitalRead(yellowButtonPin) == LOW){
    prevYellowButtonState = NOT_PRESSED;
  }

  handleLEDs();
    
}
