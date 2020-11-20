#include <Arduino.h>
#include <time.h>

#include "NTP.h"

void configTimeWithNTP(int utc_offset, bool use_daylight_savings)
{
  time_t now;
  time_t nowish = 1510592825;
  struct tm timeinfo;

  uint8_t DST = int(use_daylight_savings);

  Serial.print("Setting time using SNTP");
  configTime(utc_offset * 3600, DST * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);

  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }

  Serial.println("done!");

  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

char *getTimestampAscii(void)
{
  static struct tm timeinfo;
  static time_t now;

  // Get current timestamp (ie. "Tue Apr 21 21:30:31 2020\n")
  now = time(nullptr);
  gmtime_r(&now, &timeinfo);

  return asctime(&timeinfo);
}
