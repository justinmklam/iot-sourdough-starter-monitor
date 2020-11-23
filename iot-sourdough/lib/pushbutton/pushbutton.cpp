/* PushButton.cpp
Purpose: Class for handling switch debouncing
Last edited: August 29, 2016
Author: Justin Lam
*/

#include <Arduino.h>
#include "pushbutton.h"

// Class constructor
PushButton::PushButton(int pin) {
  switchPin = pin;
  prev_state = LOW;
  state = LOW;
  last_debounce = 0;
  debounce_delay = 50;   // Can increase this delay (in ms) if debouncing is not effective

  pinMode(pin, INPUT);
}

void PushButton::setDebounceDelay(int milliseconds) {
  debounce_delay = milliseconds;
}

// Check that the switch is actually pressed for some time to ensure it's not just noise/rattling.
// Returns true if the switch is pressed, false if not
bool PushButton::isPressed() {
  switchPressed = false;

  reading = digitalRead(switchPin);

  // If the state has changed from noise or actual pressing
  if (reading != prev_state) {
    last_debounce = millis();
  }

  // State has existed for longer than debounce_delay, so take it as actual state (not noise)
  if ((millis() - last_debounce) > debounce_delay) {
    // Return true if the switch is pressed (signal from HIGH to LOW)
    if (reading == HIGH) {
      switchPressed = true;
    }
  }
  prev_state = reading;

  return switchPressed;
}
