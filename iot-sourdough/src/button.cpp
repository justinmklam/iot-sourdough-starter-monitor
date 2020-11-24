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
            // Start of button press
            Serial.println("Pressed");
            pressTimeStart = millis();
        }
        else {
            // End of button press
            pressDuration = millis() - pressTimeStart;
            Serial.print(pressDuration);
            Serial.println(" ms pressed");
        }
    }
    else if (buttonState == HIGH) {
        // While button is being pressed
        Serial.print(millis() - pressTimeStart);
        Serial.println(" ms pressing");
    }
    prevButtonState = buttonState;
}
