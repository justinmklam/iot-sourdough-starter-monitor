#pragma once

void initializeButton();
void tUserInputCallback();
int getState();
void setState(int state);
int getDisplayState();

enum States  {
    STATE_DEFAULT,
    STATE_MONITOR,
    STATE_CALIBRATION
};

enum DisplayStates {
    DISPLAY_STATE_DEFAULT,
    DISPLAY_STATE_GRAPH,
    DISPLAY_STATE_ADVANCED,
    NUM_DISPLAY_STATES
};
