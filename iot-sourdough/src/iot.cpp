#include <AWS.h>
#include <NTP.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "userinput.h"
#include "measurements.h"
#include "secrets.h"

#define WIFI_CONNECTION_TIMEOUT_MS 30000
#define LED_PIN D4

extern Measurements measurements;

AwsIot awsClient;

bool waitUntilWifiConnected(String message)
{
  Serial.print(message);

  long start_time = millis();
  bool is_wifi_connected = true;

  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - start_time > WIFI_CONNECTION_TIMEOUT_MS) {
      is_wifi_connected = false;
      break;
    }
    Serial.print(".");
    delay(1000);
  }

  if (is_wifi_connected) {
    Serial.println("ok!");
  }
  else {
    Serial.println("timed out");
  }

  return is_wifi_connected;
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
  bool wifi_connected;

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // LED off

  WiFi.hostname("levain-monitor");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  wifi_connected = waitUntilWifiConnected(String("Attempting to connect to SSID: ") + String(ssid));

  if (!wifi_connected) {
    Serial.println("Wifi not connected. Starting in offline mode.");
    return;
  }
  else {
    digitalWrite(LED_PIN, LOW); // LED on
  }

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

    digitalWrite(LED_PIN, LOW);

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

    digitalWrite(LED_PIN, HIGH);
}
