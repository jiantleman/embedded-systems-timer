#include <Stepper.h>

const int CLOCKFREQ = 32000;

typedef enum {
  sSTARTING = 1,
  sRUNNING = 2,
  sPAUSED = 3,
  sFINISHED = 4
} state;

// Ports
int STOP_VAL = 95;
int START_VAL = 120;
int INPUT_START_PAUSE = 6;
int INPUT_RESET_BUTTON = 7;
int INPUT_INC_BUTTON = 8;
int INPUT_DEC_BUTTON = 9;
int OUTPUT_LED_0 = 0;
int OUTPUT_LED_1 = 1;

int STEPS_PER_REV = 32;
Stepper myStepper(STEPS_PER_REV,2,3,4,5);
int RPM = 50;


// Indexes for button_array
int START_PAUSE_BUTTON = 0;
int RESET_BUTTON = 1;
int INC_BUTTON = 2;
int DEC_BUTTON = 3;

// Input variables
volatile int button_array[4];
int timer;
float freq_step;
float dist_travelled;
state CURRENT_STATE;
float TRACK_DIST = 10; //TODO

// Function declarations
state update_fsm(state CURRENT_STATE); //, int timer, volatile int button_array[4], float freq_step, float dist_travelled);
void start_step(float freq_step);
void stop_step();
void reset_system();
void clear_buttons(int button);
void set_lights(int timer);
void WDT_handler(); // Watchdog
void TC3_handler(); // Timer/counter
void start_pause_handler(); 
void reset_button_handler(); 
void inc_button_handler(); 
void dec_button_handler(); 
