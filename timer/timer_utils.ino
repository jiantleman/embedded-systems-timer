void clear_buttons(){
  button_array[0] = 0;
  button_array[1] = 0;
  button_array[2] = 0;
  button_array[3] = 0;
}

void start_pause_handler(){
  button_array[START_PAUSE] = 1;
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
