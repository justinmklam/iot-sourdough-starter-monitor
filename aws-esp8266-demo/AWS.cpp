#include "AWS.h"

#include <time.h>
#include "FS.h"

const int MQTT_PORT = 8883;

AwsIot::AwsIot()
{
  net = new WiFiClientSecure();
  client = new PubSubClient(*net);
}

void AwsIot::pubSubErr(int8_t MQTTErr)
{
  if (MQTTErr == MQTT_CONNECTION_TIMEOUT)
    Serial.print("Connection tiemout");
  else if (MQTTErr == MQTT_CONNECTION_LOST)
    Serial.print("Connection lost");
  else if (MQTTErr == MQTT_CONNECT_FAILED)
    Serial.print("Connect failed");
  else if (MQTTErr == MQTT_DISCONNECTED)
    Serial.print("Disconnected");
  else if (MQTTErr == MQTT_CONNECTED)
    Serial.print("Connected");
  else if (MQTTErr == MQTT_CONNECT_BAD_PROTOCOL)
    Serial.print("Connect bad protocol");
  else if (MQTTErr == MQTT_CONNECT_BAD_CLIENT_ID)
    Serial.print("Connect bad Client-ID");
  else if (MQTTErr == MQTT_CONNECT_UNAVAILABLE)
    Serial.print("Connect unavailable");
  else if (MQTTErr == MQTT_CONNECT_BAD_CREDENTIALS)
    Serial.print("Connect bad credentials");
  else if (MQTTErr == MQTT_CONNECT_UNAUTHORIZED)
    Serial.print("Connect unauthorized");
}

void AwsIot::setThingname(const char *name)
{
  thingname = name;
}

void AwsIot::setShadowTopic(const char *topic)
{
  shadowTopic = topic;
}

void AwsIot::setPublishTopic(const char *topic)
{
  publishTopic = topic;
}

void AwsIot::setSubscribeTopic(const char *topic)
{
  subscribeTopic = topic;
}

void AwsIot::setCallback(MQTT_CALLBACK_SIGNATURE)
{
  client->setCallback(callback);
}
void AwsIot::connect()
{
  Serial.print("MQTT connecting... ");
  while (!client->connected())
  {
    if (client->connect(thingname))
    {
      Serial.println("connected!");
      if (subscribeTopic != NULL && !client->subscribe(subscribeTopic))
        pubSubErr(client->state());
    }
    else
    {
      Serial.print("failed, reason -> ");
      pubSubErr(client->state());
      Serial.println(" < trying again in 5 seconds...");
      delay(5000);
    }
  }
}

void AwsIot::loadCertificatesFromSPIFFS(void)
{
  if (!SPIFFS.begin())
  {
    Serial.println("ERROR: Failed to mount file system");
    return;
  }

  // Load certificate file
  File cert = SPIFFS.open("/cert.der", "r"); //replace cert.crt eith your uploaded file name
  if (!cert)
  {
    Serial.print("ERROR: Failed to open cert file");
  }
  else
    Serial.print("Opened cert file... ");

  delay(1000);

  if (net->loadCertificate(cert))
    Serial.println("OK!");
  else
    Serial.println("ERROR");

  // Load private key file
  File private_key = SPIFFS.open("/private.der", "r"); //replace private eith your uploaded file name
  if (!private_key)
  {
    Serial.print("ERROR: Failed to open private cert file");
  }
  else
    Serial.print("Opened private cert file... ");

  delay(1000);

  if (net->loadPrivateKey(private_key))
    Serial.println("OK!");
  else
    Serial.println("ERROR");

  // Load CA file
  File ca = SPIFFS.open("/ca.der", "r"); //replace ca eith your uploaded file name
  if (!ca)
  {
    Serial.print("ERROR: Failed to open ca ");
  }
  else
    Serial.print("Opened ca file... ");

  delay(1000);

  if (net->loadCACert(ca))
    Serial.println("OK!");
  else
    Serial.println("ERROR");
}

void AwsIot::publishMessage(StaticJsonDocument<200> doc)
{
  char msg[measureJson(doc) + 1];
  serializeJson(doc, msg, sizeof(msg));

  Serial.printf("Sending [%s]: ", publishTopic);
  Serial.println(msg);

  if (!client->publish(publishTopic, msg))
    pubSubErr(client->state());
}

void AwsIot::updateDeviceShadow(const char *message)
{
  Serial.printf("Sending [%s]: ", shadowTopic);
  Serial.println(message);

  if (!client->publish(shadowTopic, message))
    pubSubErr(client->state());
}

void AwsIot::setHost(const char *name)
{
  client->setServer(name, MQTT_PORT);
}

void AwsIot::loop()
{
  client->loop();
}

bool AwsIot::connected()
{
  return client->connected();
}
