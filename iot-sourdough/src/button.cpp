#include <Arduino.h>

#include <pushbutton.h>
#include "button.h"

#define BUTTON_PIN 9

PushButton button(BUTTON_PIN);

void initializeButton() {
    // button.setDebounceDelay(100);
}

void tButtonCallback() {
    static int buttonState = 0;
    static int prevButtonState = 0;
    static long pressTimeStart = millis();
    static long pressDuration;

    buttonState = button.isPressed();

    if (prevButtonState != buttonState) {
        if (buttonState == HIGH) {
            Serial.println("Pressed");
            pressTimeStart = millis();
        }
        else {
            pressDuration = millis() - pressTimeStart;
            Serial.print(pressDuration);
            Serial.println(" ms pressed");
        }
    }
    prevButtonState = buttonState;
}
