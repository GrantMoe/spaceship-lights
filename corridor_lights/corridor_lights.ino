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
int last_flash[NUM_LEDS];

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

String input_mode;
String current_mode;

light_state current_state;

light_state red_alert = {CRGB(255, 0, 0), 0, 127, 1000, false};
light_state orange_alert = {CRGB(255, 24, 0), 8, 127, 2000, true};
light_state yellow_alert = {CRGB(255, 90, 0), 16, 127, 3000, true};
light_state no_alert = {CRGB(255,255,255), 3, 4, 5000, true};
light_state broken_state = {CRGB(255,255,255), 0, 4, 5000, true};

light_state test_alert = {CRGB(255, 20, 150), 8, 8, 5000, true};


void setup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  input_mode = "red_alert";
  current_state = no_alert;

  input_mode = "broken_state";
//  Serial.begin(9600);
}

void loop() {

  if (current_mode != input_mode) {

    current_mode = input_mode;

    if (input_mode == "red_alert") {
      current_state = red_alert;
    } else if (input_mode == "orange_alert") {
      current_state = orange_alert;
    } else if (input_mode == "yellow_alert") {
      current_state = yellow_alert;
    } else if (input_mode == "custom_alert") {
      // ADD CODE HERE
    } else if (input_mode == "broken_state") {
      current_state = broken_state;
      for (int i = 0; i < NUM_LEDS; i++) {
        flash_number[i] = 0;
        flash_duration[i] = 0;
      }
    
    } else {
      current_state = no_alert;
    }
  }

  if (current_mode == "broken_state") {
    for (int i = 0; i < NUM_LEDS; i++) {
      if (random(50000) <= 1) {
        flash_number[i] += random(5);
        flash_duration[i] = random(50, 1000);
      } 
    }
  }

  current_time = millis();

  // set/reset cycle time
  if ((current_time - cycle_start) > current_state.period_duration) {
    cycle_start = current_time; 
  }

  // calculate brightness based percentage of cycle, scaled to 8 bits
  wave_index = ((float(current_time) - float(cycle_start)) / float(current_state.period_duration)) * 256;

//  Serial.println(wave_index);

  if (current_mode == "broken_state") {
    for (int i = 0; i < NUM_LEDS; i++) {

      if (flash_number[i] > 0) {
        if ( (float(current_time) - float(last_flash[i])) > float(flash_duration[i]) ) {
          last_flash[i] = current_time;
          flash_number[i] = flash_number[i] - 1;
        }
        wave_index = ((float(current_time) - float(last_flash[i])) / float(flash_duration[i])) * 256;
        leds[i] = blend(CRGB::Black, current_state.color, 
                                    map(cubicwave8(wave_index), 0, 255, current_state.min_value, current_state.max_value ));
      } else {
        leds[i] = CRGB::Black;
      }     
    }
  } else {
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
  } 

  // show LEDs
  FastLED.show();
//  delay(10); // needed?
}
