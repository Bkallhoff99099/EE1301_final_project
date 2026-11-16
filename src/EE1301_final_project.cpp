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

int pixel_count = 3;
#define PIXEL_PIN SPI
int PIXEL_TYPE = WS2812;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixel_count, PIXEL_PIN, PIXEL_TYPE);


// setup() runs once, when the device is first turned on
void setup() {
  pinMode(redButtonPin, INPUT_PULLDOWN);
  pinMode(blueButtonPin, INPUT_PULLDOWN);
  pinMode(greenButtonPin, INPUT_PULLDOWN);
  pinMode(yellowButtonPin, INPUT_PULLDOWN);
  strip.begin();
  // Put initialization like pinMode and begin functions here
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

  // Example: Publish event to cloud every 10 seconds. Uncomment the next 3 lines to try it!
  // Log.info("Sending Hello World to the cloud!");
  // Particle.publish("Hello world!");
  // delay( 10 * 1000 ); // milliseconds and blocking - see docs for more info!
}
