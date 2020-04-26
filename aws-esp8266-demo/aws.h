#pragma once

#include <PubSubClient.h>
#include <WiFiClientSecure.h>

class AwsIot
{
public:
    AwsIot();
    void loadCertificatesFromSPIFFS(void);
    void setHost(const char* name);
    void setThingname(const char* name);
    void setShadowTopic(const char* topic);
    void setPublishTopic(const char* name);
    void setSubscribeTopic(const char* name);
    void setCallback(MQTT_CALLBACK_SIGNATURE);
    void updateDeviceShadow(const char* message);
    void connect();
    void loop();
    bool connected();
    void publishMessage();
private:
    WiFiClientSecure *net;
    PubSubClient *client;

    void pubSubErr(int8_t MQTTErr);

    const char* thingname;
    const char* shadowTopic;
    const char* publishTopic;
    const char* subscribeTopic;
};
