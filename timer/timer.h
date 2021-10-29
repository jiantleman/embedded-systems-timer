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
int OUTPUT_SERVO = 2;

// Indexes for button_array
int START_PAUSE = 6;
int RESET_BUTTON = 7;
int INC_BUTTON = 8;
int DEC_BUTTON = 9;

// Input variables
volatile int button_array[4];
int timer;
float freq_step;
state CURRENT_STATE;

// Function declarations
int update_fsm(state CURRENT_STATE, int timer, int button_array[4], float freq_step);
void start_step(int freq);
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
