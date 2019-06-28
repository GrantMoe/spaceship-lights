// Program for spaceship corridor lighting (alerts, emergency, etc)

#include <FastLED.h>

#define DATA_PIN    0
#define CLOCK_PIN   1
#define LED_TYPE  APA102
#define COLOR_ORDER BGR
#define NUM_LEDS 30
#define NUM_FLICKERING 6

CRGB leds[NUM_LEDS];
int flash_number[NUM_LEDS];
int flash_duration[NUM_LEDS];
int last_flash[NUM_LEDS];


// hand-pick lights for flicker states (for continuity)
int flickering_leds[6] = {1, 6, 11, 16, 21, 26};
int flicker_number[NUM_FLICKERING];
int flicker_duration[NUM_FLICKERING];
int last_flicker[NUM_FLICKERING];

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
int mode_index = 7;  // START OUT OF RANGE TO RESET TO 0 (LAZY)
String modes[7] = {"no_alert", "yellow_alert", "orange_alert",
                    "red_alert", "test_alert", "broken_state", "flicker_state"};
light_state current_state;



// color, min_value, max_value, period_duration, throb
light_state red_alert = {CRGB(255, 0, 0), 0, 127, 1000, false};
light_state orange_alert = {CRGB(255, 48, 0), 8, 127, 2000, true};
light_state yellow_alert = {CRGB(255, 128, 0), 16, 127, 3000, true};
light_state no_alert = {CRGB(255, 255, 255), 127, 255, 5000, true};
light_state broken_state = {CRGB(255, 255, 255), 0, 255, 5000, true};
light_state flicker_state = {CRGB(255, 255, 255), 32, 32, 5000, true};

light_state test_alert = {CRGB(64, 128, 0), 0, 127, 1000, false};


void setup() {
  FastLED.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(4)>(leds, NUM_LEDS);

  input_mode = "flicker_state";
  current_mode = "no_alert";

//  input_mode = "test_alert";
//  Serial.begin(9600);

  // set input mode-cycle pin
  pinMode(5, INPUT_PULLUP);

}

void loop() {
  
  // Mode selection/update
  // Check user input button
  if (digitalRead(5) == LOW) {
          delay(500); // crude debounce
    if (++mode_index >= 7) {
      mode_index = 0;
    }
    input_mode = modes[mode_index];
  }
  if (current_mode != input_mode) {
    current_mode = input_mode;
    if (input_mode == "red_alert") {
      current_state = red_alert;
    } else if (input_mode == "orange_alert") {
      current_state = orange_alert;
    } else if (input_mode == "yellow_alert") {
      current_state = yellow_alert;
    } else if (input_mode == "test_alert") {
      current_state = test_alert;
    } else if (input_mode == "broken_state") {
      current_state = broken_state;
      for (int i = 0; i < NUM_LEDS; i++) {
        flash_number[i] = 0;
        flash_duration[i] = 0;
      }    
    } else if (input_mode == "flicker_state") {
      for (int i = 0; i < NUM_FLICKERING; i++) {
        current_state = flicker_state;
        flicker_number[i] = 0;
        flicker_duration[i] = 0;
      }
    } else {
      current_state = no_alert;
    }
  }

  // set/reset cycle time
  current_time = millis();
  if ((current_time - cycle_start) > current_state.period_duration) {
    cycle_start = current_time; 
  }

  // broken state
  if (current_mode == "broken_state") {
    for (int i = 0; i < NUM_LEDS; i++) {
      // add random duration, randomly distributed flashes
      if (random(100000) <= 1) {
        flash_number[i] += random(5);
        flash_duration[i] = random(50, 1000);
      } 
      // flash as necessary if light is flashing
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
  } else { // non-broken (whole strip) state
    
    // calculate brightness based percentage of cycle, scaled to 8 bits
    wave_index = ((float(current_time) - float(cycle_start)) / float(current_state.period_duration)) * 256;
    
    // assign LED colors and throbbing brightness level
    if (current_state.throb == true) {
      // use trig function to determine throb brightness
      fill_solid(leds, NUM_LEDS, blend(CRGB::Black, 
                                    current_state.color, 
                                    map(cubicwave8(wave_index), 0, 255, current_state.min_value, current_state.max_value )));
    } else {
      // calculate hard half-on/half-off cycle phase
      if (wave_index >= 127) {  // for half-on, half-off without throb
        fill_solid(leds, NUM_LEDS, blend(CRGB::Black, current_state.color, current_state.max_value));  
      } else {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
      }
    }

    // flicker the lights in need of flickering
    if (current_mode == "flicker_state") {
      for (int i = 0; i < NUM_FLICKERING; i++) {
        // add dimming flickers randomly
        if (random(50000) <= 1) {
          flicker_number[i] += random(5);
          flicker_duration[i] = random(50, 500);
        }
        // flicker as necessary
        if (flicker_number[i] > 0) {
          if ( (float(current_time) - float(last_flicker[i])) > float(flicker_duration[i]) ) {
            last_flicker[i] = current_time;
            flicker_number[i] = flicker_number[i] - 1;
          }
        wave_index = ((float(current_time) - float(last_flicker[i])) / float(flicker_duration[i])) * 256;
        leds[flickering_leds[i]] = blend(CRGB::Black, current_state.color,
                                    map(cubicwave8(wave_index), 0, 255, 0, current_state.max_value ));

        }
      }
    }
  } 

  // show LEDs
  FastLED.show();
//  delay(10); // if needed
}
