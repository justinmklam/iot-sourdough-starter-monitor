#include <Arduino.h>

#include <pushbutton.h>
#include "button.h"

#define BUTTON_PIN 9
#define LONG_PRESS_THRESHOLD_MS 300

int userState = STATE_DEFAULT;

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
        pressDuration = millis() - pressTimeStart;
        Serial.print(pressDuration);
        Serial.println(" ms pressing");

        if (pressDuration >= LONG_PRESS_THRESHOLD_MS) {
            if (userState != STATE_CALIBRATION) {
                Serial.println("Entering calibration");
                userState = STATE_CALIBRATION;
            }
        }
    }
    prevButtonState = buttonState;
}
