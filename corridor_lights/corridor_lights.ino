// Program for spaceship corridor lighting (alerts, emergency, etc)

#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUMPIXELS 30 // Number of LEDs in strip

// Control pins:
#define DATAPIN    0
#define CLOCKPIN   1

// Create Dotstar strip object
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

// Hardware SPI is a little faster, but must be wired to specific pins
// Teensy 3.2 = pin 11 for MOSI (alt: 7), 13 for clock (alt: 14).
//Adafruit_DotStar strip(NUMPIXELS, DOTSTAR_BGR);

void setup() {
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
}

// create packed colors with strip.Color(R, G, B)
uint32_t color = strip.Color(255, 255, 255);
uint32_t red = strip.Color(255, 0, 0);
uint32_t orange = strip.Color(255, 128, 0);
uint32_t yellow = strip.Color(255, 255, 0);
uint32_t white = strip.Color(255, 255, 255);

int min_brightness = 0;
int max_brightness = 16;
int current_brightness = 0;
int sine_index = 0;
float hz = 0.5;
int loop_delay = 1000 / (hz * 255);

void loop() {

  if (++sine_index >= 256) {
    sine_index = 0;
  }

  current_brightness = map(strip.sine8(sine_index),
                            0, 
                            255, 
                            min_brightness, 
                            max_brightness
                            );

    strip.setBrightness(current_brightness);

  strip.fill(strip.gamma32(color), 0, NUMPIXELS);  
  strip.show();                     // Refresh strip
  delay(loop_delay);                        

}
