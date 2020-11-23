#pragma once

/*PushButton.h
Purpose: Class for handling switch debouncing
Last edited: August 29, 2016
Author: Justin Lam
*/

class PushButton{
  private:
    int switchPin;      // hardware pin on dev board
    int reading;
    int prev_state;
    int state;
    bool switchPressed;
    unsigned long last_debounce;
    unsigned long debounce_delay;

  public:
    PushButton(int pin);     // constructor to initialize variables
    bool isPressed();  // Returns true if switch is pressed, false if not
    void setDebounceDelay(int milliseconds);
};
