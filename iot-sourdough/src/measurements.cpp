#include "Adafruit_VL6180X.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <EEPROM.h>

#include "measurements.h"
#include "userinput.h"

// mins * seconds * milliseconds
#define BUFFER_UPDATE_INTERVAL_MS 5 * 60 * 1000
#define DHTPIN 10
#define DHTTYPE DHT22
#define EEPROM_ADDR_JAR_HEIGHT 0
#define EEPROM_ADDR_SESSION_ID 4

CircularBuffer<float, 128> bufferRiseHeight;

Measurements measurements;

Adafruit_VL6180X vl = Adafruit_VL6180X();
DHT_Unified dht(DHTPIN, DHTTYPE);
int jarHeightMm = 0;

void initializeMeasurements()
{
  if (!vl.begin())
  {
    Serial.println("Failed to find sensor");
    while (1)
      ;
  }
  Serial.println("VL6180X sensor found!");

  randomSeed(analogRead(A0));
  dht.begin();
  sensor_t dht_sensor;
  dht.temperature().getSensor(&dht_sensor);

  EEPROM.begin(512);
  EEPROM.get(EEPROM_ADDR_JAR_HEIGHT, jarHeightMm);
  Serial.print("Jar height: ");
  Serial.println(jarHeightMm);
  measurements.jarHeightMm = jarHeightMm;
}

void tMeasureCallback()
{
  static int levainHeightMm = 0;
  static int currentState = STATE_DEFAULT;
  static int prevState = STATE_DEFAULT;

  measurements.range = vl.readRange();
  measurements.status = vl.readRangeStatus();

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  measurements.temperature = event.temperature;
  dht.humidity().getEvent(&event);
  measurements.humidity = event.relative_humidity;

  if (levainHeightMm != 0)
  {
    measurements.rise_height = jarHeightMm - measurements.range - levainHeightMm;
    measurements.rise_percent = 100.0 * (float)measurements.rise_height / (float)levainHeightMm;
  }
  else
  {
    measurements.rise_height = 0;
    measurements.rise_percent = 0;
  }

  currentState = getState();
  switch (currentState)
  {
  case STATE_CALIBRATION:
    jarHeightMm = measurements.range;
    measurements.jarHeightMm = jarHeightMm;

    break;
  case STATE_MONITOR:
    static long prevBufferUpdateTimeMs = 0;

    // Only add to the buffer ever N seconds since the graph should capture the whole rise in one screen
    if (millis() - prevBufferUpdateTimeMs > BUFFER_UPDATE_INTERVAL_MS)
    {
      bufferRiseHeight.push(measurements.rise_percent);

      for (int i = 0; i < bufferRiseHeight.size(); i++)
      {
        if (bufferRiseHeight[i] > measurements.maxRisePercent)
        {
          measurements.maxRisePercent = bufferRiseHeight[i];
          measurements.timeOfMaxRiseMs = millis();
        }
      }
      prevBufferUpdateTimeMs = millis();
    }

    measurements.timeSinceMaxRiseMins = (millis() - measurements.timeOfMaxRiseMs) / 60000.0;

    break;
  case STATE_DEFAULT:
    levainHeightMm = 0;
    break;
  }

  // Save jar height to EEPROM when exiting calibration
  if (prevState == STATE_CALIBRATION && currentState != STATE_CALIBRATION)
  {
    EEPROM.put(EEPROM_ADDR_JAR_HEIGHT, jarHeightMm);

    if (!EEPROM.commit())
    {
      Serial.println("EEPROM ERROR! Commit failed");
    }
    else
    {
      Serial.print("Saved to EEPROM: ");
      Serial.println(jarHeightMm);
    }
  }

  if (prevState != currentState)
  {
    // Start of new monitoring session
    if (currentState == STATE_MONITOR)
    {
      EEPROM.get(EEPROM_ADDR_SESSION_ID, measurements.sessionId);
      measurements.sessionId++;
      levainHeightMm = jarHeightMm - measurements.range;
      bufferRiseHeight.clear();
      measurements.timeOfMaxRiseMs = millis();
      measurements.maxRisePercent = 0;
      measurements.timeSinceMaxRiseMins = 0;
      measurements.sessionStartTimeMs = millis();

      EEPROM.put(EEPROM_ADDR_SESSION_ID, measurements.sessionId);
      if (!EEPROM.commit())
      {
        Serial.println("EEPROM ERROR! Commit failed");
      }
      else
      {
        Serial.print("Saved session ID to EEPROM: ");
        Serial.println(measurements.sessionId);
      }

      measurements.isMonitoring = true;
    }
    else if (currentState == STATE_DEFAULT)
    {
      measurements.isMonitoring = false;
    }
  }
  prevState = currentState;
}
