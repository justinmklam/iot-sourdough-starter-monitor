#include <Arduino.h>

#include <OneButton.h>
#include "userinput.h"

#define BUTTON_PIN 9

int userState = STATE_DEFAULT;
int displayState = DISPLAY_STATE_DEFAULT;

// False since active high
OneButton button(BUTTON_PIN, false);

void onLongPress() {
    Serial.println("Long press");

    switch (userState) {
        case STATE_DEFAULT:
            // Start monitoring
            userState = STATE_MONITOR;
            break;
        case STATE_MONITOR:
            // Stop monitoring
            userState = STATE_DEFAULT;
            break;
    }
}

void onDoubleClick() {
    Serial.println("Double click");

    switch (userState) {
        case STATE_DEFAULT:
            Serial.println("Entering calibration");
            userState = STATE_CALIBRATION;
            break;
    }
}

void onClick() {
    Serial.println("Click");

    switch (userState) {
        case STATE_CALIBRATION:
            // Short button press exits calibration mode
            userState = STATE_DEFAULT;
            break;
        default:
            // Cycle through display states
            displayState++;
            if (displayState == NUM_DISPLAY_STATES) {
                displayState = 0;
            }
            break;
    }
}

void initializeButton() {
    button.attachClick(onClick);
    button.attachDoubleClick(onDoubleClick);
    button.attachLongPressStart(onLongPress);
}

void tUserInputCallback() {
    button.tick();
}

int getState() {
    return userState;
}

void setState(int state) {
    userState = state;
}

int getDisplayState() {
    return displayState;
}
