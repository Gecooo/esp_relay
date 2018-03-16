/*
 * MQTT functions
 */

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("MQTT message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  char* topicBody = topic + mqttClient.length() + 1; // Skip "/ClientName" from topic
  if (! strncmp(topicBody, mqttTopic.c_str(), mqttTopic.length())) {
    switch ((char)payload[0]) {
      case '0':
        switchRelay(false);
        break;
      case '1':
        switchRelay(true);
        break;
      default:
        bool relay = digitalRead(relayPin);
        if (! relayLevel)
          relay = ! relay;
        mqtt_publish(pubsubClient, String(topic), String(relay));
    }
  } else {
    Serial.println(F("Unexpected topic!"));
  }
}

bool mqttReconnect() {
  const uint32_t timeout = 30000;
  static uint32_t lastTime;
  bool result = false;

  if (millis() > lastTime + timeout) {
    Serial.print(F("Attempting MQTT connection..."));
    digitalWrite(pinBuiltinLed, LOW);
    if (mqttUser.length())
      result = pubsubClient.connect(mqttClient.c_str(), mqttUser.c_str(), mqttPassword.c_str());
    else
      result = pubsubClient.connect(mqttClient.c_str());
    digitalWrite(pinBuiltinLed, HIGH);
    if (result) {
      Serial.println(F(" connected"));
      // Resubscribe
      String topic('/');
      topic += mqttClient;
      topic += mqttTopic;
      result = mqtt_subscribe(pubsubClient, topic);
    } else {
      Serial.print(F(" failed, rc="));
      Serial.println(pubsubClient.state());
    }
    lastTime = millis();
  }

  return result;
}

bool mqtt_subscribe(PubSubClient& client, const String& topic) {
  Serial.print(F("Subscribing to "));
  Serial.println(topic);

  return client.subscribe(topic.c_str());
}

bool mqtt_publish(PubSubClient& client, const String& topic, const String& value) {
  Serial.print(F("Publishing topic "));
  Serial.print(topic);
  Serial.print(F(" = "));
  Serial.println(value);

  return client.publish(topic.c_str(), value.c_str());
}

