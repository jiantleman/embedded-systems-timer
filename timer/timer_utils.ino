// Mocked variables for display testing
int mocked_timer;
float mocked_freq_step;

void clear_buttons(){
  button_array[0] = 0;
  button_array[1] = 0;
  button_array[2] = 0;
  button_array[3] = 0;
}

void start_pause_handler(){
  #ifdef TESTING
  Serial.println("Start/pause button clicked");
  #endif
  button_array[START_PAUSE_BUTTON] = 1;
}

void reset_button_handler(){
  #ifdef TESTING
  Serial.println("Reset button clicked");
  #endif
  button_array[RESET_BUTTON] = 1;
}

void inc_button_handler(){
  #ifdef TESTING
  Serial.println("Increment button clicked");
  #endif
  button_array[INC_BUTTON] = 1;
}

void dec_button_handler(){
  #ifdef TESTING
  Serial.println("Decrement button clicked");
  #endif
  button_array[DEC_BUTTON] = 1;
}

#ifndef TESTING

void set_lights(int timer){
  Serial.print("Set lights: ");
  Serial.print(timer);
  Serial.println(" min");
  
  int value = timer - 1;
  if (value & 1){
    digitalWrite(OUTPUT_LED_0, HIGH);
  } else {
    digitalWrite(OUTPUT_LED_0, LOW);
  }
  if (value & 2){
    digitalWrite(OUTPUT_LED_1, HIGH);
  } else {
    digitalWrite(OUTPUT_LED_1, LOW);
  }
}

void start_step(float freq_step){
    Serial.print("Start step: ");
    Serial.println(freq_step);

    // Turn off interrupts to TC3 on MC0 when configuring
    TC3->COUNT16.INTENCLR.reg = TC_INTENCLR_MC0; 
    TC3->COUNT16.CTRLA.reg = TC_CTRLA_ENABLE | TC_CTRLA_PRESCSYNC(1) | TC_CTRLA_PRESCALER(0) | TC_CTRLA_WAVEGEN(1) | TC_CTRLA_MODE(0);
    while(TC3->COUNT16.STATUS.bit.SYNCBUSY);
    TC3->COUNT16.CC[0].reg = CLOCKFREQ/freq_step;
    while(TC3->COUNT16.STATUS.bit.SYNCBUSY);
    // Turn interrupts to TC3 on MC0 back on when done configuring
    TC3->COUNT16.INTENSET.reg = TC_INTENSET_MC0;   
}

void stop_step(){
    Serial.println("Stop step");
    TC3->COUNT16.CTRLA.bit.ENABLE = 0;
    TC3->COUNT16.INTENCLR.reg = TC_INTENCLR_MC0;
}

void reset_system(){
//    myStepper.setSpeed(RPM);
//    for (int i=0;i < steps_taken; i++) {
//      myStepper.step(-1);
//    }
    Serial.println("Reset system");
    steps_taken = 0;
    stepper.runToNewPosition(0);
    stepper.setCurrentPosition(0);
    stepper.moveTo(TRACK_DIST);
}

#else

// Mocked functions for testing

void set_lights(int timer){
  Serial.print("Set lights: ");
  Serial.print(timer);
  Serial.println(" min");
  mocked_timer = timer;
}

void start_step(float freq_step){
    Serial.print("Start step: ");
    Serial.println(freq_step);
    mocked_freq_step = freq_step;
}

void stop_step(){
    Serial.println("Stop step");
}

void reset_system(){
    Serial.println("Reset system");
}
#endif

void TC3_Handler() {
  Serial.println("Step start");
  // Clear interrupt register flag
  TC3->COUNT16.INTFLAG.reg |= TC_INTFLAG_MC0; 
//  myStepper.setSpeed(RPM);
//  myStepper.step(1);
  steps_taken += 1;
  stepper.run();
  Serial.println("Step finish");
}

void WDT_Handler() {
  // Clear interrupt register flag
  WDT->INTFLAG.reg |= WDT_INTFLAG_EW;
  // Warn user that a watchdog reset may happen
  Serial.println("WDT Reset May Happen");
}

void print_state(state CURRENT_STATE) {
  if (prevState != CURRENT_STATE) {
    prevState = CURRENT_STATE;
    switch (CURRENT_STATE) {
      case 1: Serial.println("sSTARTING"); break;
      case 2: Serial.println("sRUNNING"); break;
      case 3: Serial.println("sPAUSED"); break;
      case 4: Serial.println("sFINISHED"); break;
      default: Serial.println("ERROR"); break;
    }
  }
} 
