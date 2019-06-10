// Program for spaceship corridor lighting (alerts, emergency, etc)

#include <FastLED.h>

#define DATA_PIN    0
#define CLOCK_PIN   1
#define LED_TYPE  APA102
#define COLOR_ORDER BGR
#define NUM_LEDS 30 // Number of LEDs in strip
CRGB leds[ NUM_LEDS ];

void setup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
}

void loop() {

  fill_solid(leds, NUM_LEDS, CHSV(0,255,64));
  FastLED.show();
  delay(1000);

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(1000);                        

}
