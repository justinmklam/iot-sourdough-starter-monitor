#pragma once

void initializeMeasurements();
void tMeasureCallback();

typedef struct Measurements {
  uint8_t range = 0;
  uint8_t status = 0;
  float temperature = 0;
  float humidity = 0;
  float rise_percent = 0;
} Measurements;
