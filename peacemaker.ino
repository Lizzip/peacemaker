#include <Adafruit_NeoPixel.h>

// Gun
#define GUN_SPREAD_SPEED 1.2
bool READY_TO_FIRE = true;

// LED Strip
#define LED_COUNT 19
#define LED_PIN 2
#define LED_FREQ_HZ NEO_GRB + NEO_KHZ800
#define LED_BRIGHTNESS 155
#define LED_OFFSET 10


// Predefined Colours 
int OFF[] = {0, 0, 0};
int YELLOW[] = {255, 175, 0};
int ORANGE[] = {255, 55, 0};
int DIM_ORANGE[] = {125, 25, 0};

// Button
const int buttonPin = 3;
int buttonState = 0;

//create a NeoPixel strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, LED_FREQ_HZ);


void setup() {
  // put your setup code here, to run once:
  
  // start the strip and blank it out
  strip.setBrightness(LED_BRIGHTNESS);
  strip.begin();
  strip.show();


  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    cocking();
  } else {
    shoot();
  }
}

void fade (int pixels[2], int colour1[3], int colour2[3], int steps, int interval) {

  for (int i = 1; i <= steps; i++) {
    int r = ((colour1[0] * (steps - i)) + (colour2[0] * i)) / steps;
    int g = ((colour1[1] * (steps - i)) + (colour2[1] * i)) / steps;
    int b = ((colour1[2] * (steps - i)) + (colour2[2] * i)) / steps;

    strip.setPixelColor(pixels[0], r, g, b);
    strip.setPixelColor(pixels[1], r, g, b);
    
    strip.show();
    delay(interval);
  }
}

void shoot () {

  // Turn off each LED pair individually starting from gun end (to simulate symbols going out with the bullet) 
  const int wipe_speed = 15;
  
  for (int i = (LED_COUNT - 1)/2; i >= 0; i--) {
    strip.setPixelColor(((LED_COUNT - LED_OFFSET) + i), OFF[0], OFF[1], OFF[2]);
    strip.setPixelColor(((LED_COUNT - LED_OFFSET) - i), OFF[0], OFF[1], OFF[2]);
    strip.show();
    delay(wipe_speed);
  }

  READY_TO_FIRE = true;
}

void cocking (){
  
  if (READY_TO_FIRE) {
    int steps = 65;
    int interval = 30;

    // Flag gun not ready to fire again (until it's been shot)
    READY_TO_FIRE = false;
    const int yellow_strength = 110;

    // Start tip of barrel as DIM_ORANGE
    strip.setPixelColor(LED_COUNT- LED_OFFSET, DIM_ORANGE[0], DIM_ORANGE[1], DIM_ORANGE[2]);
    strip.show();
    delay(GUN_SPREAD_SPEED);

    // Fade tip of barrel to YELLOW
    int pixels[2] = {LED_COUNT- LED_OFFSET, 100}; //100 because we only want to fade 1 pixel
    int step_yellow[3] = {YELLOW[0], yellow_strength, YELLOW[2]};
    fade(pixels, DIM_ORANGE, step_yellow, steps, interval); 

    //Spread fade up barrel at increasing speed
    interval = 1;
    for (int i = 1; i <= ((LED_COUNT - 1)/2); i++) {

      // Tint lights towards orange the further up the barrel they are
      int fading_yellow_strength = yellow_strength - (i*7);
      int decreasing_steps = steps - (i*4);
      int pixelPair[2] = {(LED_COUNT - LED_OFFSET) + i, (LED_COUNT - LED_OFFSET) - i};
      int decreased_yellow[3] = {YELLOW[0], fading_yellow_strength, YELLOW[2]};
      fade(pixelPair, DIM_ORANGE, decreased_yellow, decreasing_steps, interval);
    }
  }
}
