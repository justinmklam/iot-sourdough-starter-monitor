#pragma once

void initializeDisplay();
void tDisplayCallback();
void setNextDisplayState();

enum DisplayStates {
    DISPLAY_STATE_DEFAULT,
    DISPLAY_STATE_GRAPH,
    DISPLAY_STATE_GRAPH_TEMPERATURE,
    DISPLAY_STATE_ADVANCED,
    NUM_DISPLAY_STATES
};
