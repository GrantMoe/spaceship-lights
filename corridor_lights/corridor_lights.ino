// Program for spaceship corridor lighting (alerts, emergency, etc)

#include <FastLED.h>

#define DATA_PIN    0
#define CLOCK_PIN   1
#define LED_TYPE  APA102
#define COLOR_ORDER BGR
#define NUM_LEDS 30

CRGB leds[NUM_LEDS];
int flash_number[NUM_LEDS];
int flash_duration[NUM_LEDS];

unsigned long current_time = millis();
unsigned long cycle_start = current_time;

int wave_index = 0;

struct light_state {
  CRGB color;
  int min_value;
  int max_value;
  unsigned long period_duration;
  bool throb;
};

light_state red_alert = {CRGB(255, 0, 0), 0, 127, 1000, false};
light_state orange_alert = {CRGB(255, 24, 0), 8, 127, 2000, true};
light_state yellow_alert = {CRGB(255, 90, 0), 16, 127, 3000, true};
light_state no_alert = {CRGB(255,255,255), 3, 4, 5000, true};
light_state test_alert = {CRGB(255, 20, 150), 8, 8, 5000, true};

light_state current_state;

void setup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  current_state = red_alert;

//  Serial.begin(9600);
}

void loop() {

  current_time = millis();

  // set/reset cycle time
  if ((current_time - cycle_start) > current_state.period_duration) {
    cycle_start = current_time;
  }

  // calculate brightness based percentage of cycle, scaled to 8 bits
  wave_index = ((float(current_time) - float(cycle_start)) / float(current_state.period_duration)) * 256;

//  Serial.println(wave_index);

  // set LED colors
    if (current_state.throb) {
      fill_solid(leds, NUM_LEDS, blend(CRGB::Black, 
                                    current_state.color, 
                                    map(cubicwave8(wave_index), 0, 255, current_state.min_value, current_state.max_value )));
    } else {
      if (wave_index >= 127) {  // for half-on, half-off without throb
        fill_solid(leds, NUM_LEDS, blend(CRGB::Black, current_state.color, current_state.max_value));  
      } else {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
      }
    }

  // show LEDs
  FastLED.show();
//  delay(10); // needed?
}
