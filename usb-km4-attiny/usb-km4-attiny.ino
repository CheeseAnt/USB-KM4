#define BUTTON_4 A0
#define BUTTON_3 A1
#define BUTTON_2 A2
#define BUTTON_1 A3
#define CEN_4 A7
#define CEN_3 8
#define CEN_2 9
#define CEN_1 10

#define AUTO_EN_THRESH 910
#define SW_EN_THRESH_H 700
#define SW_EN_THRESH_L 650

#define AUTO_EN_PULSE 1500 // time to wait between checking for auto enables

int but_vals[] = {0, 0, 0, 0};

int buttons[] = {
  BUTTON_1,
  BUTTON_2,
  BUTTON_3,
  BUTTON_4
};

int cens[] = {
  CEN_1,
  CEN_2,
  CEN_3,
  CEN_4
};

int enabled_usb = -1;
int auto_timer = 0;

void readButtons() {
  /* Read and update the voltages for each button */
  for(int i=0; i<4; i++) {
    but_vals[i] = analogRead(buttons[i]);
  }
}

int autoOnPin() {
  /* Returns -1 on cannot make automatic selection or a single usb number if one is enabled
    Cycles through all pins, seeing if they are in the AUTO_EN_THRESH state. If only one is,
    return that pin. Otherwise return -1.*/
  int auto_pin = -1;
  
  for(int i=0; i<4; i++) {
    bool button_auto = (but_vals[i] > AUTO_EN_THRESH);
    
    if(button_auto & (auto_pin == -1)) {
      auto_pin = i;
    }
    else if(button_auto & (auto_pin != -1)) {
      return -1;
    }
  }

  return auto_pin;
}

int checkButtonPress() {
  /* Very similar to auto select. Check all buttons and see which match the threshold,
    if more than one match then return -1. If only one matches, return that choice. */
  int chosen_pin = -1;
  
  for(int i=0; i<4; i++) {
    bool button_pressed = (but_vals[i] < SW_EN_THRESH_H)
      & (but_vals[i] > SW_EN_THRESH_L);

    // single button press
    if(button_pressed & (chosen_pin == -1)) {
      chosen_pin = i;
    }
    // more than one button pressed
    else if(button_pressed & (chosen_pin != -1)) {
      return -1;
    }
  }

  return chosen_pin;
}

int enableUSB(int new_usb) {
  /* enable the new usb port, checking to not double enable */

  if((new_usb != enabled_usb) & (new_usb != -1)) {
    if(enabled_usb != -1) {
      digitalWrite(cens[enabled_usb], LOW);
    }
    
    digitalWrite(cens[new_usb], HIGH);

    // overwrite currently enabled
    enabled_usb = new_usb;

    // reset the auto enable timer
    auto_timer = millis();
  }
}

void setup() {
  /* Pull all pins low for input, then enable CEN pins as outputs */
  pinMode(CEN_1, INPUT);
  pinMode(CEN_2, INPUT);
  pinMode(CEN_3, INPUT);
  pinMode(CEN_4, INPUT);
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_4, INPUT);

  /* Enable the CEN as outputs */
  pinMode(CEN_1, OUTPUT);
  pinMode(CEN_2, OUTPUT);
  pinMode(CEN_3, OUTPUT);
  pinMode(CEN_4, OUTPUT);
}

void loop() {
  int result = -1;

  // update button values
  readButtons();
  
  // check for any actual presses
  result = checkButtonPress();

  if((result == -1) & ((millis() - auto_timer) > AUTO_EN_PULSE)) {
      result = autoOnPin();
  }
  
  // enable whichever port was chosen
  enableUSB(result);
}
