// Program for spaceship corridor lighting (alerts, emergency, etc)

#include <FastLED.h>

#define DATA_PIN    0
#define CLOCK_PIN   1
#define LED_TYPE  APA102
#define COLOR_ORDER BGR
#define NUM_LEDS 30 // Number of LEDs in strip
CRGB leds[ NUM_LEDS ];

int wave_index = 0;

struct alert_state {
  CRGB color;
  long period;
  int min_value;
  int max_value;
};

alert_state red_alert = {CRGB(255, 0, 0), 1000, 0, 127};
alert_state orange_alert = {CRGB(255, 24, 0), 2000, 8, 127};
alert_state yellow_alert = {CRGB(255, 90, 0), 3000, 16, 127};
alert_state no_alert = {CRGB(255,255,255), 5000, 3, 4 };
alert_state test_alert = {CRGB(255, 20, 150), 5000, 8, 8};

alert_state current_state;

void setup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  current_state = test_alert;
}

void loop() {

  if (++wave_index >= 256) {
    wave_index = 0;
  }

  fill_solid(leds, NUM_LEDS, blend(CRGB::Black, 
                                  current_state.color, 
                                  map( cubicwave8(wave_index), 0, 255, current_state.min_value, current_state.max_value ) ) );

  FastLED.show();
  delay(current_state.period/255);
}
