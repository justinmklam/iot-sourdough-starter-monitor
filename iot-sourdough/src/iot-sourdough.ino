#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <TaskScheduler.h>

#include <AWS.h>
#include <NTP.h>

#include "display.h"
#include "measurements.h"
#include "secrets.h"

// If false, device will be standalone/offline
#define ENABLE_IOT false

#if ENABLE_IOT
AwsIot awsClient;
#endif

Task tMeasure(100, TASK_FOREVER, &tMeasureCallback);
Task tDisplay(250, TASK_FOREVER, &tDisplayCallback);

Scheduler taskManager;

#if ENABLE_IOT
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
#endif

void setup()
{
  Serial.begin(115200);
  Serial.println("AWS Levain Monitor");
  delay(100);

  initializeMeasurements();
  initializeDisplay();

  taskManager.init();
  taskManager.addTask(tMeasure);
  taskManager.addTask(tDisplay);
  tMeasure.enable();
  tDisplay.enable();

#if ENABLE_IOT
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
#endif
}

void loop() {
  taskManager.execute();
}
// void loop()
// {
//   static unsigned long lastMillisMeasure = 0;
// #if ENABLE_IOT
//   static unsigned long lastMillisPublish = 0;
//   StaticJsonDocument<200> publishMessage;
//   static char shadowMessage[50];
// #endif
//   static uint8_t range = 0;
//   static uint8_t status = 0;
//   static float temperature = 0;
//   static float humidity = 0;

//   if (millis() - lastMillisMeasure > 100)
//   {
//     lastMillisMeasure = millis();
//     range = vl.readRange();
//     status = vl.readRangeStatus();

//     sensors_event_t event;
//     dht.temperature().getEvent(&event);
//     temperature = event.temperature;
//     dht.humidity().getEvent(&event);
//     humidity = event.relative_humidity;

//     display.clearDisplay();
//     display.setCursor(0,0);
//     if (status == VL6180X_ERROR_NONE) {
//       // Serial.print("Range: "); Serial.println(range);
//       display.print(range);
//       display.print("mm\n");
//     } else {
//       display.print("n/a\n");
//       // Serial.print("Error reading VL6180X, code: ");
//       // Serial.println(status);
//       // return;
//     }
//     display.print(temperature);
//     display.print("C\n");
//     display.print(humidity);
//     display.print("%");
//     display.display();
//   }

// #if ENABLE_IOT
//   if (!awsClient.connected())
//   {
//     waitUntilWifiConnected("Checking WiFi");
//     awsClient.connect();
//   }
//   else
//   {
//     awsClient.loop();
//     if (millis() - lastMillisPublish > 60000)
//     {
//       lastMillisPublish = millis();

//       publishMessage["time"] = getTimestampAscii();
//       publishMessage["temperature"] = temperature;
//       publishMessage["humidity"] = humidity;
//       publishMessage["distance"] = range;

//       awsClient.publishMessage(publishMessage);

//       // sprintf(shadowMessage, "{\"state\":{\"reported\": {\"range\": %ld}}}", range);
//       // awsClient.updateDeviceShadow(shadowMessage);
//     }
//   }
// #endif
// }
