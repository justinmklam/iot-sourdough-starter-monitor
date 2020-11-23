#include <Arduino.h>

#include <pushbutton.h>
#include "button.h"

#define BUTTON_PIN 9

PushButton button(BUTTON_PIN);

void initializeButton() {
}

void tButtonCallback() {
    static int buttonState = 0;

    buttonState = button.isPressed();

    if (buttonState == HIGH) {
        Serial.println("Pressed");
    }
}
