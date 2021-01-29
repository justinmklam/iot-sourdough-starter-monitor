#pragma once

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

class AwsIot
{
public:
  AwsIot();

  void loadCertificatesFromSPIFFS(void);
  void loadCertificates(const BearSSL::X509List *cert, const BearSSL::X509List *chain, const BearSSL::PrivateKey *sk);

  void setHost(const char *name);
  void setThingname(const char *name);
  void setShadowTopic(const char *topic);
  void setPublishTopic(const char *name);
  void setSubscribeTopic(const char *name);
  void setCallback(MQTT_CALLBACK_SIGNATURE);

  void updateDeviceShadow(const char *message);
  bool publishMessage(StaticJsonDocument<200> doc);

  void loop();
  void connect();
  void disconnect();
  bool connected();

private:
  WiFiClientSecure *net;
  PubSubClient *client;

  void pubSubErr(int8_t MQTTErr);

  const char *thingname;
  const char *shadowTopic;
  const char *publishTopic;
  const char *subscribeTopic;
};
