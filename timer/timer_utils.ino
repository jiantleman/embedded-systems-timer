void clear_buttons(int button){
  button_array[button] = 0;
}

void start_pause_handler(){
  button_array[START_PAUSE_BUTTON] = 1;
}

void reset_button_handler(){
  button_array[RESET_BUTTON] = 1;
}

void inc_button_handler(){
  button_array[INC_BUTTON] = 1;
}

void dec_button_handler(){
  button_array[DEC_BUTTON] = 1;
}

void set_lights(int timer){
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

void start_step(float freq_step){ //TODO
    Serial.println(freq_step);
    // Turn off interrupts to TC3 on MC0 when configuring
    TC3->COUNT16.INTENCLR.reg = TC_INTENCLR_MC0; 
    TC3->COUNT16.CTRLA.reg = TC_CTRLA_ENABLE | TC_CTRLA_PRESCSYNC(1) | TC_CTRLA_PRESCALER(0) | TC_CTRLA_WAVEGEN(1) | TC_CTRLA_MODE(0);
    while(TC3->COUNT16.STATUS.bit.SYNCBUSY);
    TC3->COUNT16.CC[0].reg = CLOCKFREQ/freq_step;
    while(TC3->COUNT16.STATUS.bit.SYNCBUSY);
    TC3->COUNT16.INTENSET.reg = TC_INTENSET_MC0; 
    // Turn interrupts to TC3 on MC0 back on when done configuring
}

void stop_step(){//TODO
    Serial.println("Stopped");
    TC3->COUNT16.CTRLA.bit.ENABLE = 0;
    TC3->COUNT16.INTENCLR.reg = TC_INTENCLR_MC0;
//    PORT->Group[PORTB].DIRCLR.reg != (1<<10);
}

void TC3_Handler() {
  // Clear interrupt register flag
  TC3->COUNT16.INTFLAG.reg |= TC_INTFLAG_MC0; 
  dist_travelled += 1;
  myStepper.setSpeed(RPM);
  myStepper.step(1);
}

void reset_system(){//TODO
    Serial.println("Reset");
}
