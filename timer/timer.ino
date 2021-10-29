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
  freq_step = timer;
}

void loop() {
  // put your main code here, to run repeatedly:
  // CURRENT_STATE = update_fsm(CURRENT_STATE, timer, button_array, freq_step);
  
}

int update_fsm(state CURRENT_STATE, int timer, int button_array[4], float freq_step){
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
    } else {
      next_state = sSTARTING;    
    }
    break;
  case sRUNNING:
    break;
  case sPAUSED:
    break;
  case sFINISHED:
    break;
  }
  return next_state;
}
