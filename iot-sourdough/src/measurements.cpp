#include "Adafruit_VL6180X.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "measurements.h"
#include "userinput.h"

#define DHTPIN 10
#define DHTTYPE DHT22

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

float calculate_rise_percent(int levainHeight, int jarHeight, int currentHeight) {
  float x = levainHeight - currentHeight;
  float y = jarHeight - levainHeight;

  return x / y * 100.0;
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
    measurements.rise_percent = calculate_rise_percent(levainHeightMm, jarHeightMm, measurements.range);
  }
  else {
    measurements.rise_percent = 0;
  }

  Serial.print(measurements.range);
  Serial.print("mm, ");
  Serial.print(measurements.temperature);
  Serial.print("C, ");
  Serial.print(measurements.humidity);
  Serial.print("%, ");
  Serial.print(measurements.rise_percent);
  Serial.print("%\n");

  switch (getState()) {
    case STATE_CALIBRATION:
      jarHeightMm = measurements.range;
      break;
    case STATE_MONITOR:
      if (levainHeightMm == 0) {
        levainHeightMm = measurements.range;
      }
      break;
    case STATE_DEFAULT:
      levainHeightMm = 0;
      break;
  }
}
