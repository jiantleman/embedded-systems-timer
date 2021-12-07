#include "timer.h"
//#define TESTING
void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Configuring pins");
  // Configure pins
  pinMode(INPUT_START_PAUSE, INPUT);
  pinMode(INPUT_RESET_BUTTON, INPUT);
  pinMode(INPUT_INC_BUTTON, INPUT);
  pinMode(INPUT_DEC_BUTTON, INPUT);
  pinMode(OUTPUT_LED_0, OUTPUT);
  pinMode(OUTPUT_LED_1, OUTPUT);

  // Initial input and variable values
  timer = 1;
  freq_step = TRACK_DIST/(timer*60);
  set_lights(timer);
  CURRENT_STATE = sSTARTING;
  prevState = sSTARTING;

  clear_buttons();
  steps_taken = 0;
  stepper.setCurrentPosition(0);  
  stepper.moveTo(TRACK_DIST);
  stepper.setMaxSpeed(1000.0);
  stepper.setAcceleration(100.0);

  Serial.println("Setting up interrupts");
  // Set up interrupts
  attachInterrupt(digitalPinToInterrupt(INPUT_START_PAUSE), start_pause_handler, RISING);
  attachInterrupt(digitalPinToInterrupt(INPUT_RESET_BUTTON), reset_button_handler, RISING);
  attachInterrupt(digitalPinToInterrupt(INPUT_INC_BUTTON), inc_button_handler, RISING);
  attachInterrupt(digitalPinToInterrupt(INPUT_DEC_BUTTON), dec_button_handler, RISING);

  Serial.println("Setting up Timer/Counter");
  // Configure and enable GCLK4 for TC:
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(0) | GCLK_GENDIV_ID(4);
  while(GCLK->STATUS.bit.SYNCBUSY);
  GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC(5) | GCLK_GENCTRL_ID(4);
  while(GCLK->STATUS.bit.SYNCBUSY);
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(4) | GCLK_CLKCTRL_ID(0x1b);

  // Check if APB is enabled:
  Serial.print("Check if APB Enabled: ");
  Serial.print(0, BIN);
  Serial.print(" ");
  Serial.println(PM->APBCMASK.reg & 1<<11 );

  // Configure and enable TC:
  TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while(TC3->COUNT16.STATUS.bit.SYNCBUSY);
  TC3->COUNT16.INTENCLR.reg = TC_INTENCLR_MC0; 
  while(TC3->COUNT16.STATUS.bit.SYNCBUSY);

  NVIC_SetPriority(TC3_IRQn, 1);
  NVIC_EnableIRQ(TC3_IRQn);
  
  Serial.println("Setting up WDT");
  // Clear and enable WDT
  NVIC_DisableIRQ(WDT_IRQn);
  NVIC_ClearPendingIRQ(WDT_IRQn);
  NVIC_SetPriority(WDT_IRQn, 0);
  NVIC_EnableIRQ(WDT_IRQn);

  // Configure and enable WDT GCLK:
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(4) | GCLK_GENDIV_ID(5);
  while (GCLK->STATUS.bit.SYNCBUSY);
  GCLK->GENCTRL.reg = GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC(5) | GCLK_GENCTRL_ID(5);
  while(GCLK->STATUS.bit.SYNCBUSY);
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(5) | GCLK_CLKCTRL_ID(0x3);
  while(GCLK->STATUS.bit.SYNCBUSY);
  
  // Configure and enable WDT:
  WDT->CONFIG.reg = WDT_CONFIG_PER(0xB);
  WDT->EWCTRL.reg = WDT_EWCTRL_EWOFFSET(0xA);
  WDT->CTRL.reg = WDT_CTRL_ENABLE;

  // Enable early warning interrupts on WDT:
  WDT->INTENSET.reg = WDT_INTENSET_EW;
  
  Serial.println("Starting System");
  Serial.println("sSTARTING");
//  test_fsm();
//  test_button_handlers();
//  test_leds();
//  test_stepper();
}

void loop() {
   CURRENT_STATE = update_fsm(CURRENT_STATE, button_array, steps_taken);
   clear_buttons();

   WDT->CLEAR.reg = 0xa5;
}

state update_fsm(state CURRENT_STATE, volatile int local_button_array[4], volatile int local_steps_taken){
  state next_state;
  print_state(CURRENT_STATE);
  switch(CURRENT_STATE) {
  case sSTARTING:
    if(local_button_array[INC_BUTTON]==1 && timer < 4 && 
       local_button_array[DEC_BUTTON]==0 && 
       local_button_array[START_PAUSE_BUTTON]==0){                  // Transition 1-1a
      set_lights(timer+1);
      timer = timer+1;
      freq_step = TRACK_DIST/(timer*60);
      next_state = sSTARTING; 
    } else if (local_button_array[DEC_BUTTON]==1 && timer > 1 && 
       local_button_array[INC_BUTTON]==0 && 
       local_button_array[START_PAUSE_BUTTON]==0){                  // Transition 1-1b
      set_lights(timer-1);
      timer = timer - 1;
      freq_step = TRACK_DIST/(timer*60);
      next_state = sSTARTING; 
    } else if (local_button_array[START_PAUSE_BUTTON]==1){          // Transition 1-2
      start_step(freq_step);
      next_state = sRUNNING; 
    } else {                                                  
      next_state = sSTARTING;    
    }
    break;
  case sRUNNING:
    if (local_button_array[START_PAUSE_BUTTON]==1 &&
        local_steps_taken != TRACK_DIST){                           // Transition 2-3
      stop_step();
      next_state = sPAUSED;
    } else if (local_steps_taken == TRACK_DIST){                    // Transition 2-4
      stop_step();
      next_state = sFINISHED;                             
    } else{
      next_state = sRUNNING;
    }
    break;
  case sPAUSED:
    if (local_button_array[RESET_BUTTON]==1){                        // Transition 3-1
        reset_system();
        next_state = sSTARTING;
      } else if (local_button_array[START_PAUSE_BUTTON]==1 &&
                 local_button_array[RESET_BUTTON]==0){               // Transition 3-2
        start_step(freq_step);
        next_state = sRUNNING;
      } else{
        next_state = sPAUSED;
      }
    break;
  case sFINISHED:
    if(local_button_array[RESET_BUTTON]==1){                         // Transition 4-1
      reset_system();
      next_state = sSTARTING;
    } else {
      next_state = sFINISHED;
    }
    break;
  }
  return next_state;
}
