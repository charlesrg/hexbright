/*
Copyright (c) 2013, "David Hilton" <dhiltonp@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
*/

#include <print_power.h>

// These next two lines must come after all other library #includes
#define BUILD_HACK
#include <hexbright.h>

hexbright hb;

#define OFF_TIME 650 // milliseconds before going off on the next normal button press

int levels[] = {0,300,700,1000};
int mode = 0;
int modes = 6; // off,low,med,high,blink bright ramp, fade blink ramp, blink ramp
unsigned long short_press_time = 0;

void setup() {
  hb.init_hardware(); 
}

void loop() {
  hb.update();
  if(hb.button_just_released()) {
    
    if(hb.button_pressed_time()<OFF_TIME) {
      // we just had a normal duration press
      mode++;
      if (mode > modes ){
        mode=0;
        hb.set_light(CURRENT_LEVEL, 0, NOW);
      }
    }
    else{
      hb.set_light(CURRENT_LEVEL, 0, NOW);
      mode = 0;
    }
  }
  if ( mode > 0 && mode < 4 ){ //low, med , high
    hb.set_light(CURRENT_LEVEL, levels[mode], 500);
  }
  if ( mode == 4 ){ // ramp up brightness and blink
    static unsigned long flash_time = millis();
    static unsigned long start_delay = 100;
    static unsigned long low_delay = 0;
    static unsigned long cur_delay = start_delay;
    static int lowBright = 200;
    static int highBright = 1000;
    static int interval = 5;
    static int brightSteps = (highBright - lowBright) / int((start_delay - low_delay)/ long(interval));
    static int curBright = lowBright - brightSteps;
    static boolean lightState = true;
    if(flash_time+cur_delay<millis()) { // flash every x milliseconds
      flash_time = millis(); // reset flash_time
      cur_delay-= long(interval);
      if(cur_delay <= low_delay) {
        cur_delay=start_delay;
        curBright=lowBright-brightSteps;
      }
      if(lightState == false) {
        curBright += brightSteps;
        hb.set_light(0, curBright, NOW); // and pulse (going from max to min over 20 milliseconds)
        lightState=true;
      }else {
        hb.set_light(CURRENT_LEVEL, 0, NOW);
        lightState=false;
      }
      //hb.set_light(0, MAX_LEVEL, 30); // and pulse (going from max to min over 20 milliseconds)
      // actually, because of the refresh rate, it's more like 'go from max brightness on high
      //  to max brightness on low to off.
    }
  }
  if ( mode == 5 ){ // ramp up brightness without off
    static unsigned long flash_time = millis();
    static unsigned long start_delay = 100;
    static unsigned long low_delay = 0;
    static unsigned long cur_delay = start_delay;
    static int lowBright = 200;
    static int highBright = 1000;
    static int interval = 5;
    static int brightSteps = (highBright - lowBright) / int((start_delay - low_delay)/ long(interval));
    static int curBright = lowBright - brightSteps;
    static boolean lightState = true;
    if(flash_time+cur_delay<millis()) { // flash every x milliseconds
      flash_time = millis(); // reset flash_time
      cur_delay-= long(interval);
      if(cur_delay <= low_delay) {
        cur_delay=start_delay;
        curBright=lowBright-brightSteps;
      }
      if(lightState == false) {
        curBright += brightSteps;
        hb.set_light(CURRENT_LEVEL, curBright, NOW); // and pulse (going from max to min over 20 milliseconds)
        lightState=true;
      }else {
        hb.set_light(CURRENT_LEVEL, curBright, NOW);
        lightState=false;
      }
      //hb.set_light(0, MAX_LEVEL, 30); // and pulse (going from max to min over 20 milliseconds)
      // actually, because of the refresh rate, it's more like 'go from max brightness on high
      //  to max brightness on low to off.
    }
  } 
  if ( mode == 6 ){ // ramp up
    static unsigned long flash_time = millis();
    static unsigned long delay = 500;
    static int lowBright = 200;
    static int highBright = 1000;
    if(flash_time+delay<millis()) { // ramp every x milliseconds
      hb.set_light(lowBright, highBright, delay); // and pulse (going from max to min over 20 milliseconds)
      flash_time = millis();
    }
  }
}

