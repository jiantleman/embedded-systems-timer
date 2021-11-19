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

  // Initial variable values
  timer = 1;
  freq_step = (timer*60)/TRACK_DIST;
  dist_travelled = 0;
  set_lights(timer);
  clear_buttons(INC_BUTTON);
  clear_buttons(DEC_BUTTON);
  clear_buttons(START_PAUSE_BUTTON);
  clear_buttons(RESET_BUTTON);
  CURRENT_STATE = sSTARTING;

  // Set up interrupts
  attachInterrupt(digitalPinToInterrupt(INPUT_START_PAUSE), start_pause_handler, RISING);
  attachInterrupt(digitalPinToInterrupt(INPUT_RESET_BUTTON), reset_button_handler, RISING);
  attachInterrupt(digitalPinToInterrupt(INPUT_INC_BUTTON), inc_button_handler, RISING);
  attachInterrupt(digitalPinToInterrupt(INPUT_DEC_BUTTON), dec_button_handler, RISING);

  // Configure and enable GCLK4 for TC:
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(0) | GCLK_GENDIV_ID(4); // do not divide gclk 4
  while(GCLK->STATUS.bit.SYNCBUSY);
  GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC(3) | GCLK_GENCTRL_ID(4);
  while(GCLK->STATUS.bit.SYNCBUSY);
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(4) | GCLK_CLKCTRL_ID(0x1b);

  // Check if APB is enabled:
  Serial.println(0, BIN); // FIXME: PM->APBX.reg (change X to the correct letter account to the prelab)
  Serial.println(PM->APBCMASK.reg & 1<<11 );

  // Configure and enable TC:
  TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while(TC3->COUNT16.STATUS.bit.SYNCBUSY);
  TC3->COUNT16.INTENCLR.reg = TC_INTENCLR_MC0; 
  while(TC3->COUNT16.STATUS.bit.SYNCBUSY);
  

}


void loop() {
   CURRENT_STATE = update_fsm(CURRENT_STATE);
}

state update_fsm(state CURRENT_STATE){
  state next_state;
  switch(CURRENT_STATE) {
  case sSTARTING:
    if(button_array[INC_BUTTON]==1 && timer < 4 && 
       button_array[DEC_BUTTON]!=1 && 
       button_array[START_PAUSE_BUTTON]!=1){                  // Transition 1-1a
      set_lights(timer+1);
      clear_buttons(INC_BUTTON);
      timer = timer+1;
      freq_step = (timer*60)/TRACK_DIST;
      next_state = sSTARTING; 
    } else if (button_array[DEC_BUTTON]==1 && timer > 1 && 
       button_array[INC_BUTTON]!=1 && 
       button_array[START_PAUSE_BUTTON]!=1){                  // Transition 1-1b
      set_lights(timer-1);
      clear_buttons(DEC_BUTTON);
      timer = timer - 1;
      freq_step = (timer*60)/TRACK_DIST;
      next_state = sSTARTING; 
    } else if (button_array[START_PAUSE_BUTTON]==1){          // Transition 1-2
      start_step(freq_step);
      clear_buttons(START_PAUSE_BUTTON);
      next_state = sRUNNING; 
    } else {                                                  // Transition 1-1c
      clear_buttons(INC_BUTTON);
      clear_buttons(DEC_BUTTON);
      next_state = sSTARTING;    
    }
    break;
  case sRUNNING:
    if (button_array[START_PAUSE_BUTTON]==1 &&
        dist_travelled != TRACK_DIST){                    // Transition 2-3
      stop_step();
      clear_buttons(START_PAUSE_BUTTON);
      clear_buttons(RESET_BUTTON);
      next_state = sPAUSED;
    } else if (dist_travelled == TRACK_DIST){                  // Transition 2-4
      stop_step();
      clear_buttons(RESET_BUTTON);
      next_state = sFINISHED;                             
    } else{
      next_state = sRUNNING;
    }
    break;
  case sPAUSED:
    if (button_array[RESET_BUTTON]==1){                        // Transition 3-1
        reset_system();
        clear_buttons(INC_BUTTON);
        clear_buttons(DEC_BUTTON);
        clear_buttons(START_PAUSE_BUTTON);
        dist_travelled = 0;
        next_state = sSTARTING;
      } else if (button_array[START_PAUSE_BUTTON]==1 &&
                 button_array[RESET_BUTTON]!=1){               // Transition 3-2
        start_step(freq_step);
        clear_buttons(RESET_BUTTON);
        next_state = sRUNNING;
      } else{
        next_state = sPAUSED;
      }
    break;
  case sFINISHED:
    if(button_array[RESET_BUTTON]==1){                         // Transition 4-1
      reset_system();
      dist_travelled = 0;
      clear_buttons(INC_BUTTON);
      clear_buttons(DEC_BUTTON);
      clear_buttons(START_PAUSE_BUTTON);
      next_state = sSTARTING;
    } else {
      next_state = sFINISHED;
    }
    break;
  }
  return next_state;
}
