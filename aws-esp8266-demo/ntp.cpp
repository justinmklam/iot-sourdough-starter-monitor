#include <Arduino.h>
#include <time.h>

#include "ntp.h"

void NTPConnect(void)
{
  time_t now;
  time_t nowish = 1510592825;
  struct tm timeinfo;

#ifdef USE_SUMMER_TIME_DST
  uint8_t DST = 1;
#else
  uint8_t DST = 0;
#endif

  Serial.print("Setting time using SNTP");
  configTime(UTC_OFFSET * 3600, DST * 3600, "pool.ntp.org", "time.nist.gov");
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
