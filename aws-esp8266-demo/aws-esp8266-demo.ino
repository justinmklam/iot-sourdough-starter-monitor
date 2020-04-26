#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "aws.h"
#include "ntp.h"

#include "secrets.h"

AwsIot awsClient;

void waitUntilWifiConnected(String message)
{
  Serial.print(message);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("ok!");
}

void messageReceivedCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }

  Serial.println();
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  WiFi.hostname("levain-monitor");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  waitUntilWifiConnected(String("Attempting to connect to SSID: ") + String(ssid));

  NTPConnect(-7, false);

  awsClient.setThingname("levain-monitor");
  awsClient.setPublishTopic("esp8266/data");
  awsClient.setHost("a292sjcsigiv8t-ats.iot.us-west-2.amazonaws.com");
  awsClient.loadCertificatesFromSPIFFS();
  // client.setCallback(messageReceivedCallback);

  awsClient.connect();
}

void loop()
{
  static unsigned long lastMillis = 0;

  if (!awsClient.connected())
  {
    waitUntilWifiConnected("Checking WiFi");
    awsClient.connect();
  }
  else
  {
    awsClient.loop();
    if (millis() - lastMillis > 5000)
    {
      lastMillis = millis();
      awsClient.publishMessage();
    }
  }
}
