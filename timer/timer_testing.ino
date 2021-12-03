

bool test_transition(state start_state,
                     state end_state,
                     state_inputs test_state_inputs, 
                     state_vars start_state_vars,
                     state_vars end_state_vars,
                     bool verbos);

/*        
 * Helper function for printing states
 */
char* s2str(state s) {
  switch(s) {
    case sSTARTING:
    return "(1) STARTING";
    case sRUNNING:
    return "(2) RUNNIN";
    case sPAUSED:
    return "(3) PAUSED";
    case sFINISHED:
    return "(4) FINISHED";
    default:
    return "???";
  }
}

/*
 * Given a start state, inputs, and starting values for state variables, tests that
 * update_fsm returns the correct end state and updates the state variables correctly
 * returns true if this is the case (test passed) and false otherwise (test failed)
 * 
 * Need to use "verbos" instead of "verbose" because verbose is apparently a keyword
 */
bool test_transition(state start_state,
                     state end_state,
                     state_inputs test_state_inputs,
                     state_vars start_state_vars,
                     state_vars end_state_vars,
                     bool verbos) {
  timer = start_state_vars.timer;
  freq_dist = start_state_vars.freq_step;

  state result_state = update_fsm(start_state, test_state_inputs.button_array, test_state_inputs.steps_taken);

  bool passed_test = (end_state == result_state and
                      timer == end_state_vars.timer and
                      freq_dist = end_state_vars.freq_dist);

  if (! verbos) {
    return passed_test;
  } else if (passed_test) {
    char s_to_print[200];
    sprintf(s_to_print, "Test from %s to %s PASSED", s2str(start_state), s2str(end_state));
    Serial.println(s_to_print);
    return true;
  } else {
    char s_to_print[200];
    Serial.println(s2str(start_state));
    sprintf(s_to_print, "Test from %s to %s FAILED", s2str(start_state), s2str(end_state));
    Serial.println(s_to_print);
    sprintf(s_to_print, "End state expected: %s | actual: %s", s2str(end_state), s2str(result_state));
    Serial.println(s_to_print);
    return false;
  }
}

/*
 * REPLACE THE FOLLOWING 6 LINES WITH YOUR TEST CASES
 */
//const state test_states_in[0] = {};
//const state test_states_out[0] = {};
//const state_inputs test_input[0] = {};
//const state_vars test_in_vars[0] = {};
//const state_vars test_out_vars[0] = {};
//const int num_tests = 0;

/*
 * Runs through all the test cases defined above
 */
bool test_all_tests() {
  for (int i = 0; i < num_tests; i++) {
    Serial.print("Running test ");
    Serial.println(i);
    if (!test_transition(test_states_in[i], test_states_out[i], test_input[i], test_in_vars[i], test_out_vars[i], true)) {
      return false;
    }
    Serial.println();
  }
  Serial.println("All tests passed!");
  return true;
}
