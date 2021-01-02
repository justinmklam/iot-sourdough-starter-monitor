#include <AWS.h>
#include <NTP.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "userinput.h"
#include "measurements.h"
#include "secrets.h"

extern Measurements measurements;

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

void initializeIoT() {
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
  awsClient.setShadowTopic("$aws/things/levain-monitor/shadow/update");
  // awsClient.setSubscribeTopic("$aws/things/levain-monitor/shadow/update");
  // awsClient.setCallback(messageReceivedCallback);

  awsClient.connect();
}

void tIoTCallback() {
    static char shadowMessage[50];

    if (getState() == STATE_MONITOR) {
      StaticJsonDocument<200> publishMessage;

      publishMessage["deviceId"] = measurements.deviceId;
      publishMessage["sessionId"] = measurements.sessionId;
      publishMessage["temperature"] = measurements.temperature;
      publishMessage["humidity"] = measurements.humidity;
      publishMessage["riseHeight"] = measurements.rise_height;
      publishMessage["risePercent"] = measurements.rise_percent;

      awsClient.publishMessage(publishMessage);
    }
    else {
      // Need to keep the MQTT connection alive, so just update the shadow
      sprintf(shadowMessage, "{\"state\":{\"reported\": {\"time\": %ld}}}", millis());
      awsClient.updateDeviceShadow(shadowMessage);
    }
}
