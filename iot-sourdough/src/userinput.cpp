#include <Arduino.h>

#include <OneButton.h>
#include "userinput.h"

#define BUTTON_PIN 9

int userState = STATE_DEFAULT;

// False since active high
OneButton button(BUTTON_PIN, false);

void onClick() {
    Serial.println("Pressed");

    switch (userState) {
        case STATE_CALIBRATION:
            // Short button press exits calibration mode
            userState = STATE_DEFAULT;
            break;
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

void onLongPress() {
    Serial.println("Long press");
    if (userState != STATE_CALIBRATION) {
        Serial.println("Entering calibration");
        userState = STATE_CALIBRATION;
    }
}

void onDoubleClick() {
    Serial.println("Double press");
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
