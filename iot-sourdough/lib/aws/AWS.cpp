#include "AWS.h"

#include <time.h>
#include "FS.h"

const int MQTT_PORT = 8883;

AwsIot::AwsIot()
{
  net = new WiFiClientSecure();
  client = new PubSubClient(*net);
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

void AwsIot::connect()
{
  if (thingname == NULL)
  {
    Serial.println("ERROR: Please set a thingname before connecting to MQTT.");
    return;
  }

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
  const int num_certs = 3;
  const char *certs[num_certs];
  bool load_result = false;

  certs[0] = "/cert.der";
  certs[1] = "/private.der";
  certs[2] = "/ca.der";

  if (!SPIFFS.begin())
  {
    Serial.println("ERROR: Failed to mount file system");
    return;
  }

  for (int i=0; i < num_certs; i++)
  {
    load_result = false;
    // Load certificate file
    File cert = SPIFFS.open(certs[i], "r");

    Serial.print(certs[i]);
    if (!cert)
      Serial.print(" failed to open...");
    else
      Serial.print(" opened... ");

    delay(100);

    switch(i) {
    case 0:
      load_result = net->loadCertificate(cert);
      break;
    case 1:
      load_result = net->loadPrivateKey(cert);
      break;
    case 2:
      load_result = net->loadCACert(cert);
      break;
    default:
      Serial.print("No certificate loaded... ");
    }

    if (load_result)
      Serial.println("OK!");
    else
      Serial.println("ERROR");
  }
}

void AwsIot::loadCertificates(const BearSSL::X509List *cert, const BearSSL::X509List *chain, const BearSSL::PrivateKey *sk)
{
  Serial.println("Setting anchors");
  net->setTrustAnchors(cert);
  Serial.println("Setting client RSA");
  net->setClientRSACert(chain, sk);
}

bool AwsIot::publishMessage(StaticJsonDocument<200> doc)
{
  bool success = false;

  if (publishTopic == NULL)
  {
    Serial.println("WARNING: Please set a publish topic.");
    return false;
  }

  char msg[measureJson(doc) + 1];
  serializeJson(doc, msg, sizeof(msg));

  Serial.printf("Sending [%s]: ", publishTopic);
  Serial.println(msg);

  success = client->publish(publishTopic, msg);

  if (!success)
    pubSubErr(client->state());

  return success;
}

void AwsIot::updateDeviceShadow(const char *message)
{
  if (shadowTopic == NULL)
  {
    Serial.println("WARNING: Please set a shadow topic.");
    return;
  }

  Serial.printf("Sending [%s]: ", shadowTopic);
  Serial.println(message);

  if (!client->publish(shadowTopic, message))
    pubSubErr(client->state());
}

void AwsIot::pubSubErr(int8_t MQTTErr)
{
  Serial.print("MQTT Error: ");
  if (MQTTErr == MQTT_CONNECTION_TIMEOUT)
    Serial.println("Connection timeout");
  else if (MQTTErr == MQTT_CONNECTION_LOST)
    Serial.println("Connection lost");
  else if (MQTTErr == MQTT_CONNECT_FAILED)
    Serial.println("Connect failed");
  else if (MQTTErr == MQTT_DISCONNECTED)
    Serial.println("Disconnected");
  else if (MQTTErr == MQTT_CONNECTED)
    Serial.println("Reconnected");
  else if (MQTTErr == MQTT_CONNECT_BAD_PROTOCOL)
    Serial.println("Connect bad protocol");
  else if (MQTTErr == MQTT_CONNECT_BAD_CLIENT_ID)
    Serial.println("Connect bad Client-ID");
  else if (MQTTErr == MQTT_CONNECT_UNAVAILABLE)
    Serial.println("Connect unavailable");
  else if (MQTTErr == MQTT_CONNECT_BAD_CREDENTIALS)
    Serial.println("Connect bad credentials");
  else if (MQTTErr == MQTT_CONNECT_UNAUTHORIZED)
    Serial.println("Connect unauthorized");
}
