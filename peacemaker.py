#!/usr/bin/python

# Imports
from neopixel import * 
from gpiozero import Button
from signal import pause
import time

# Gun
GUN_SPREAD_SPEED = 0.12
READY_TO_FIRE    = True

# LED strip
LED_COUNT        = 19 # Number of LEDs in LED Strip
LED_PIN          = 18 # Controlling pin of Raspberry Pi
LED_FREQ_HZ      = 800000
LED_DMA          = 5
LED_BRIGHTNESS   = 155 # 255 brightest
LED_INVERT       = False
LED_OFFSET       = 10  # Length in LEDs of barrel

# Predefined Colours
OFF              = Color(0,0,0)
YELLOW           = Color(175,255,0)
ORANGE           = Color(55,255,0)
DIM_ORANGE       = Color(25,125,0)


# Per-pixel fade (from rgb1 to rgb2)
# github.com/jgarff/rpi_ws281x/issues/202
def fade(pixels, r1,g1,b1, r2,g2,b2, steps, interval): 
  lastUpdate = time.time() - interval

  for i in range(1, steps+1):
    r = ((r1 * (steps - i)) + (r2 * i)) / steps
    g = ((g1 * (steps - i)) + (g2 * i)) / steps
    b = ((b1 * (steps - i)) + (b2 * i)) / steps

    while((time.time() - lastUpdate) < interval):
      pass

    color = Color(g,r,b)
    for pixel in pixels:
      strip.setPixelColor(pixel, color)
    strip.show()
    lastUpdate = time.time()



# Cocking (lol) action
def cocking():
  global READY_TO_FIRE

  if READY_TO_FIRE == True:
    print("cocking gun...")
    steps = 65
    interval = 0.035

    # Flag gun not ready to fire again (until it's been shot)
    READY_TO_FIRE = False
    YELLOW_STRENGTH = 110

    # Start tip of barrel as DIM_ORANGE
    strip.setPixelColor(LED_COUNT - LED_OFFSET, DIM_ORANGE)
    strip.show()
    time.sleep(GUN_SPREAD_SPEED) 

    # Fade tip of barrel to YELLOW
    fade([(LED_COUNT - LED_OFFSET)], 125,25,0, 255,YELLOW_STRENGTH,0, steps,interval)
    time.sleep(GUN_SPREAD_SPEED/2)

    # Spread fade up barrel at increased speed
    interval = 0.005
    for i in range(1,((LED_COUNT - 1)/2)+1):
    
      # Tint lights towards orange the further up the barrel they are 
      fading_yellow_strength = YELLOW_STRENGTH - (i*7)
      decreasing_steps = steps - (i*4)
      pixels = [(LED_COUNT - LED_OFFSET) + i, (LED_COUNT - LED_OFFSET) - i]
      fade(pixels, 125,25,0, 255,fading_yellow_strength,0, decreasing_steps,interval)



# Firing action
def shoot():
  global READY_TO_FIRE
  print("firing gun...")
  
  # Turn off each LED pair individually starting from gun end (to simulate symbols going out with the bullet)
  wipe_speed = 0.015
  for i in range((LED_COUNT - 1)/2, 0, -1):
    strip.setPixelColor(((LED_COUNT - LED_OFFSET) + i), OFF)
    strip.setPixelColor(((LED_COUNT - LED_OFFSET) - i), OFF)
    strip.show()
    time.sleep(wipe_speed)

  strip.setPixelColor(LED_COUNT - LED_OFFSET, OFF)
  strip.show()

  # Flag gun ready to fire again
  READY_TO_FIRE = True



# Main logic
if __name__ == '__main__':

  # Create neopixel object
  strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, LED_FREQ_HZ, LED_DMA, LED_INVERT, LED_BRIGHTNESS)

  # Initialize library
  strip.begin()
  
  # Create button on GPIO 3
  switch = Button(3)
  switch.when_pressed = cocking
  switch.when_released = shoot

  # Keep script running
  pause()