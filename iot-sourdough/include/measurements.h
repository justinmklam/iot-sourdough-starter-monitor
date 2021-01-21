#pragma once

#include <CircularBuffer.h>

void initializeMeasurements();
void tMeasureCallback();
// CircularBuffer<float, 400> getRiseHeightBuffer();

typedef struct Measurements {
  long sessionId = 0;
  long deviceId = 0;
  int jarHeightMm = 0;
  uint8_t range = 0;
  uint8_t status = 0;
  float temperature = 0;
  float humidity = 0;
  int rise_height = 0;
  float rise_percent = 0;
  float maxRisePercent = 0;
  float timeSinceMaxRiseMins = 0;
  long sessionStartTimeMs = 0;
  bool isMonitoring = false;
} Measurements;
