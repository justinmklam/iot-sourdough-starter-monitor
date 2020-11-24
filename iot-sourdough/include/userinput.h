#pragma once

void initializeButton();
void tButtonCallback();
int getState();
void setState(int state);

enum States  {
    STATE_DEFAULT,
    STATE_CALIBRATION
};
