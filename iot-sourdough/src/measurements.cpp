#include "Adafruit_VL6180X.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "measurements.h"
#include "userinput.h"

#define DHTPIN 10
#define DHTTYPE DHT22

CircularBuffer<float, 128> bufferRiseHeight;

Measurements measurements;

Adafruit_VL6180X vl = Adafruit_VL6180X();
DHT_Unified dht(DHTPIN, DHTTYPE);

void initializeMeasurements() {
  if (! vl.begin()) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("VL6180X sensor found!");

  dht.begin();
  sensor_t dht_sensor;
  dht.temperature().getSensor(&dht_sensor);
}

void tMeasureCallback() {
  static int jarHeightMm = 0;
  static int levainHeightMm = 0;

  measurements.range = vl.readRange();
  measurements.status = vl.readRangeStatus();

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  measurements.temperature = event.temperature;
  dht.humidity().getEvent(&event);
  measurements.humidity = event.relative_humidity;

  if (levainHeightMm != 0) {
    measurements.rise_height = jarHeightMm - measurements.range - levainHeightMm;
    measurements.rise_percent = 100.0 * (float) measurements.rise_height / (float) levainHeightMm;
  }
  else {
    measurements.rise_height = 0;
    measurements.rise_percent = 0;
  }

  // Serial.print(measurements.range);
  // Serial.print("mm, ");
  // Serial.print(levainHeightMm);
  // Serial.print("mm, ");
  // Serial.print(measurements.rise_height);
  // Serial.print("mm, ");
  // Serial.print(measurements.temperature);
  // Serial.print("C, ");
  // Serial.print(measurements.humidity);
  // Serial.print("%, ");
  // Serial.print(measurements.rise_percent);
  // Serial.print("%\n");

  switch (getState()) {
    case STATE_CALIBRATION:
      jarHeightMm = measurements.range;
      break;
    case STATE_MONITOR:
      // Start of new monitoring session
      if (levainHeightMm == 0) {
        levainHeightMm = jarHeightMm - measurements.range;
        bufferRiseHeight.clear();
      }

      bufferRiseHeight.push(measurements.rise_percent);

      break;
    case STATE_DEFAULT:
      levainHeightMm = 0;
      break;
  }
}

// CircularBuffer<float, 400> getRiseHeightBuffer() {
//   return bufferRiseHeight;
// }
