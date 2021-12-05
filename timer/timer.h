#include <Stepper.h>
#include <AccelStepper.h>

// States
typedef enum {
  sSTARTING = 1,
  sRUNNING = 2,
  sPAUSED = 3,
  sFINISHED = 4
} state;

// Ports
int INPUT_START_PAUSE = 6;
int INPUT_RESET_BUTTON = 7;
int INPUT_INC_BUTTON = 8;
int INPUT_DEC_BUTTON = 9;
int OUTPUT_LED_0 = 0;
int OUTPUT_LED_1 = 1;

// TC and stepper
const int CLOCKFREQ = 32768;
const int STEPS_PER_REV = 200;
//Stepper myStepper(STEPS_PER_REV, 2, 3, 4, 5);
AccelStepper stepper;
const int RPM = 100;
const int STEPS = 1;
int mov = 0;

// Indexes for button_array
int START_PAUSE_BUTTON = 0;
int RESET_BUTTON = 1;
int INC_BUTTON = 2;
int DEC_BUTTON = 3;

// Inputs
volatile int button_array[4];
volatile int steps_taken;
int TRACK_DIST = 2400; //TODO

// Variables
int timer;
float freq_step;
state CURRENT_STATE;

// For Serial Printing
state prevState;

// Function declarations
state update_fsm(state CURRENT_STATE, volatile int local_button_array[4], volatile int local_steps_taken);
void start_step(float freq_step);
void stop_step();
void reset_system();
void clear_buttons();
void set_lights(int timer);
void WDT_handler(); // Watchdog
void TC3_handler(); // Timer/counter
void start_pause_handler(); 
void reset_button_handler(); 
void inc_button_handler(); 
void dec_button_handler(); 
void print_state(state CURRENT_STATE); 
