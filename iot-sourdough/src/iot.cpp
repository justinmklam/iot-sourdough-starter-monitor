#include <AWS.h>
#include <NTP.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "userinput.h"
#include "measurements.h"
#include "secrets.h"

#define WIFI_CONNECTION_TIMEOUT_MS 60000
#define LED_PIN D4

extern Measurements measurements;

AwsIot awsClient;
bool offline_mode = false;
bool wifi_connected = false;

void ledOn() {
  digitalWrite(LED_PIN, LOW);
}

void ledOff() {
  digitalWrite(LED_PIN, HIGH);
}

bool waitUntilWifiConnected(String message)
{
  Serial.print(message);

  long start_time = millis();
  bool is_wifi_connected = true;

  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - start_time > WIFI_CONNECTION_TIMEOUT_MS) {
      offline_mode = true;
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
  pinMode(LED_PIN, OUTPUT);
  ledOff();

  WiFi.hostname("levain-monitor");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  wifi_connected = waitUntilWifiConnected("Attempting to connect to SSID: " + String(ssid));

  if (offline_mode) {
    Serial.println("Wifi not connected. Starting in offline mode.");
    return;
  }
  else {
    ledOn();
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

  digitalWrite(LED_PIN, HIGH);
}

void tIoTCallback() {
    if (offline_mode) {
      return;
    }

    static char shadowMessage[50];
    static bool publishSuccess = false;

    if (!wifi_connected) {
      waitUntilWifiConnected("Reconnecting to " + String(ssid));
    }

    if (!awsClient.connected()) {
      Serial.println("MQTT disconnected, reconnecting now.");
      awsClient.connect();
    }

    if (getState() == STATE_MONITOR) {
      ledOn();

      StaticJsonDocument<200> publishMessage;

      publishMessage["deviceId"] = measurements.deviceId;
      publishMessage["sessionId"] = measurements.sessionId;
      publishMessage["temperature"] = measurements.temperature;
      publishMessage["humidity"] = measurements.humidity;
      publishMessage["riseHeight"] = measurements.rise_height;
      publishMessage["risePercent"] = measurements.rise_percent;

      publishSuccess = awsClient.publishMessage(publishMessage);

      if (publishSuccess) {
        ledOff();
      }
      else {
        Serial.println("ERROR: Message not published!");
      }
    }
    else {
      // Need to keep the MQTT connection alive, so just update the shadow
      sprintf(shadowMessage, "{\"state\":{\"reported\": {\"time\": %ld}}}", millis());
      awsClient.updateDeviceShadow(shadowMessage);
    }
}
