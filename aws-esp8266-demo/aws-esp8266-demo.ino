#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "AWS.h"
#include "NTP.h"

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_VL6180X.h"
#include "secrets.h"

AwsIot awsClient;

Adafruit_VL6180X vl = Adafruit_VL6180X();
Adafruit_SSD1306 display = Adafruit_SSD1306();

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
  Serial.println("AWS Levain Monitor");
  delay(100);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();

  if (! vl.begin()) {
    Serial.println("Failed to find sensor");
    while (1);
  }
  Serial.println("VL6180X sensor found!");

  // text display big!
  display.setTextSize(4);
  display.setTextColor(WHITE);

  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Starting...");
  display.display();

  WiFi.hostname("levain-monitor");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  waitUntilWifiConnected(String("Attempting to connect to SSID: ") + String(ssid));

  // Pacific standard time = UTC -7
  configTimeWithNTP(-7, false);

  awsClient.setThingname("levain-monitor");
  awsClient.setPublishTopic("esp8266/data");
  awsClient.setHost("a292sjcsigiv8t-ats.iot.us-west-2.amazonaws.com");

#if LOAD_KEYS_FROM_SPIFFS
  awsClient.loadCertificatesFromSPIFFS();
#else
// Load from hardcoded keys
  BearSSL::X509List cert(cacert);
  BearSSL::X509List client_crt(client_cert);
  BearSSL::PrivateKey key(privkey);
  awsClient.loadCertificates(&cert, &client_crt, &key);
#endif

  // Optional
  // awsClient.setShadowTopic("$aws/things/levain-monitor/shadow/update");
  // awsClient.setSubscribeTopic("$aws/things/levain-monitor/shadow/update");
  // awsClient.setCallback(messageReceivedCallback);

  awsClient.connect();
}

void loop()
{
  static unsigned long lastMillisPublish = 0;
  static unsigned long lastMillisMeasure = 0;
  static StaticJsonDocument<200> publishMessage;
  static char shadowMessage[50];
  static uint8_t range = 0;
  static uint8_t status = 0;

  if (millis() - lastMillisMeasure > 10)
  {
    lastMillisMeasure = millis();
    range = vl.readRange();
    status = vl.readRangeStatus();

    if (status == VL6180X_ERROR_NONE) {
      // Serial.print("Range: "); Serial.println(range);
      display.clearDisplay();
      display.setCursor(0,0);
      display.print(range);
      display.print("mm");
      display.display();
    } else {
      display.display();
      display.clearDisplay();
      // Serial.print("Error reading VL6180X, code: ");
      // Serial.println(status);
      // return;
    }
  }

  if (!awsClient.connected())
  {
    waitUntilWifiConnected("Checking WiFi");
    awsClient.connect();
  }
  else
  {
    awsClient.loop();
    if (millis() - lastMillisPublish > 5000)
    {
      lastMillisPublish = millis();

      publishMessage["time"] = getTimestampAscii();
      publishMessage["temperature"] = random(100);
      publishMessage["humidity"] = random(100);
      publishMessage["distance"] = range;

      awsClient.publishMessage(publishMessage);

      // sprintf(shadowMessage, "{\"state\":{\"reported\": {\"range\": %ld}}}", range);
      // awsClient.updateDeviceShadow(shadowMessage);
    }
  }
}
