#include "timer.h"

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  // Configure pins
  pinMode(INPUT_START_PAUSE, INPUT);
  pinMode(INPUT_RESET_BUTTON, INPUT);
  pinMode(INPUT_INC_BUTTON, INPUT);
  pinMode(INPUT_DEC_BUTTON, INPUT);
  pinMode(OUTPUT_LED_0, OUTPUT);
  pinMode(OUTPUT_LED_1, OUTPUT);
  pinMode(OUTPUT_SERVO, OUTPUT);

  // Set up interrupts
  attachInterrupt(digitalPinToInterrupt(INPUT_START_PAUSE), start_pause_handler, RISING);
  attachInterrupt(digitalPinToInterrupt(INPUT_RESET_BUTTON), reset_button_handler, RISING);
  attachInterrupt(digitalPinToInterrupt(INPUT_INC_BUTTON), inc_button_handler, RISING);
  attachInterrupt(digitalPinToInterrupt(INPUT_DEC_BUTTON), dec_button_handler, RISING);

  // Initial variable values
  clear_buttons();
  timer = 1;
  freq_step = timer/TRACK_DIST;
  dist_travelled = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
   CURRENT_STATE = update_fsm(CURRENT_STATE, timer, button_array, freq_step, dist_travelled);
  
}

state update_fsm(state CURRENT_STATE, int timer, volatile int button_array[4], float freq_step, float dist_travelled){
  state next_state;
  switch(CURRENT_STATE) {
  case sSTARTING:
    if(button_array[INC_BUTTON] && timer < 4){             // Transition 1-1a
      set_lights(timer+1);
      clear_buttons();
      timer++;
      freq_step = timer;
      next_state = sSTARTING; 
    } else if (button_array[DEC_BUTTON] && timer > 1){     // Transition 1-1b
      set_lights(timer-1);
      clear_buttons();
      timer--;
      freq_step = timer;
      next_state = sSTARTING; 
    } else if (button_array[START_PAUSE]){                 // Transition 1-2
      start_step(freq_step);
      clear_buttons();
      next_state = sRUNNING; 
    } else {
      next_state = sSTARTING;    
    }
    break;
  case sRUNNING:
    if (button_array[START_PAUSE]){                       // Transition 2-3
      stop_step();
      clear_buttons();
      next_state = sPAUSED;
    } else if (dist_travelled == TRACK_DIST){             // Transition 2-4
      stop_step();
      clear_buttons();
      next_state = sFINISHED;                             
    } else{
      next_state = sRUNNING;
    }
    break;
  case sPAUSED:
    if (button_array[RESET_BUTTON]){                      // Transition 3-1
        reset_system();
        clear_buttons();
        dist_travelled = 0;
        next_state = sSTARTING;
      } else if (button_array[START_PAUSE]){              // Transition 3-2
        start_step(freq_step);
        clear_buttons();
        next_state = sRUNNING;
      } else{
        next_state = sPAUSED;
      }
    break;
  case sFINISHED:
    if(button_array[RESET_BUTTON]){
      reset_system();
      dist_travelled = 0;
      clear_buttons();
      next_state = sSTARTING;
    } else {
      next_state = sFINISHED;
    }
    break;
  }
  return next_state;
}
