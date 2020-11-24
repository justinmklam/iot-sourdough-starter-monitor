#pragma once

void initializeButton();
void tUserInputCallback();
int getState();
void setState(int state);

enum States  {
    STATE_DEFAULT,
    STATE_CALIBRATION
};
