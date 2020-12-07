#pragma once

#include <CircularBuffer.h>

void initializeMeasurements();
void tMeasureCallback();
// CircularBuffer<float, 400> getRiseHeightBuffer();

typedef struct Measurements {
  uint8_t range = 0;
  uint8_t status = 0;
  float temperature = 0;
  float humidity = 0;
  int rise_height = 0;
  float rise_percent = 0;
} Measurements;
