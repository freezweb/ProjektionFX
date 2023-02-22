#if !defined(__PROJEKTIONTX_MQTT_H__)
#define __PROJEKTIONTX_MQTT_H__

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "settings.h"
#include "configuration.h"

#include <BeatInfo.h>

#include "MQTTStreamFarben.h"

#include "effect.h"
#include "effect_fullcolor.h"

extern BeatInfo beatInfo;
extern MQTTStreamFarben StreamFarbe;

WiFiClient espClient;
PubSubClient client(espClient);

const char *mqttUser;
const char *mqttPassword;


void callback(char *topic, byte *payload, unsigned int length);
void reconnect();


void reconnect()
{
  if (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "PANDORIALED-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect

    if (client.connect(clientId.c_str(), config.getMQTTUser(), config.getMQTTPassword()))
    {
      Serial.println("connected");
      client.subscribe("projektiontv/stream/dj/songinfo/bpm");
      client.subscribe("stream/ledstripes/color");
      client.subscribe("stream/ledstripes/effekt");
      StreamFarbe.setStreamColor(CRGB::Green);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  String messageTemp;

  for (unsigned int i = 0; i < length; i++)
  {
    messageTemp += (char)payload[i];
  }
  if (strcmp(topic, "projektiontv/stream/dj/songinfo/bpm") == 0)
  {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, payload, length);

    double bpm = doc["bpm"];
    Serial.printf("MQTT Received BPM: %f\n", bpm);
    beatInfo.setBPM(bpm);
  }
  if (strcmp(topic, "stream/ledstripes/effekt") == 0)
  {

    char str_array[messageTemp.length()];
    messageTemp.toCharArray(str_array, messageTemp.length());

    Serial.println(strtol(str_array, NULL, 16));
    Serial.print("Change Effect: ");
    Serial.println(messageTemp.toInt());
    effectsRunner.setEffect(messageTemp.toInt());
  }

  if (strcmp(topic, "stream/ledstripes/color") == 0)
  {
    char str_array[messageTemp.length()];
    messageTemp.toCharArray(str_array, messageTemp.length());

    Serial.println(strtol(str_array, NULL, 16));

    StreamFarbe.setStreamColor(strtol(str_array, NULL, 16));
  }
}

void setupMqtt(const char *host)
{

  Serial.println("Setting Up MQTT");
  Serial.printf("- connect to %s\n", host);
  client.setServer(host, 1883);

  client.setCallback(callback);
  reconnect();

  Serial.println("MQTT Setup completed!");
}

void loopMqtt()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();
}

#endif // __PROJEKTIONTX_MQTT_H__
