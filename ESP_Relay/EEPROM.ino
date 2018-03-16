/*
 * EEPROM configuration functions
 */

uint16_t readEEPROMString(uint16_t offset, String& str) {
  char buffer[maxStrParamLength + 1];

  buffer[maxStrParamLength] = 0;
  for (byte i = 0; i < maxStrParamLength; i++) {
    if (! (buffer[i] = EEPROM.read(offset + i)))
      break;
  }
  str = String(buffer);

  return offset + maxStrParamLength;
}

uint16_t writeEEPROMString(uint16_t offset, const String& str) {
  for (byte i = 0; i < maxStrParamLength; i++) {
    if (i < str.length())
      EEPROM.write(offset + i, str[i]);
    else
      EEPROM.write(offset + i, 0);
  }

  return offset + maxStrParamLength;
}

bool readConfig() {
  uint16_t offset = 0;

  Serial.println(F("Reading config from EEPROM"));
  for (byte i = 0; i < sizeof(configSign); i++) {
    char c = pgm_read_byte(configSign + i);
    if (EEPROM.read(offset + i) != c)
      return false;
  }
  offset += sizeof(configSign);
  offset = readEEPROMString(offset, ssid);
  offset = readEEPROMString(offset, password);
  offset = readEEPROMString(offset, domain);
  offset = readEEPROMString(offset, mqttServer);
  EEPROM.get(offset, mqttPort);
  offset += sizeof(mqttPort);
  offset = readEEPROMString(offset, mqttUser);
  offset = readEEPROMString(offset, mqttPassword);
  offset = readEEPROMString(offset, mqttClient);
  offset = readEEPROMString(offset, mqttTopic);
  offset = readEEPROMString(offset, channelID);
  offset = readEEPROMString(offset, writeAPIKey);
  EEPROM.get(offset, postingInterval);
  offset += sizeof(postingInterval);
  EEPROM.get(offset, relayPin);
  offset += sizeof(relayPin);
  EEPROM.get(offset, relayLevel);
  offset += sizeof(relayLevel);
  EEPROM.get(offset, relayOnBoot);

  return true;
}

void writeConfig() {
  uint16_t offset = 0;

  Serial.println(F("Writing config to EEPROM"));
  for (byte i = 0; i < sizeof(configSign); i++) {
    char c = pgm_read_byte(configSign + i);
    EEPROM.write(offset + i, c);
  }
  offset += sizeof(configSign);
  offset = writeEEPROMString(offset, ssid);
  offset = writeEEPROMString(offset, password);
  offset = writeEEPROMString(offset, domain);
  offset = writeEEPROMString(offset, mqttServer);
  EEPROM.put(offset, mqttPort);
  offset += sizeof(mqttPort);
  offset = writeEEPROMString(offset, mqttUser);
  offset = writeEEPROMString(offset, mqttPassword);
  offset = writeEEPROMString(offset, mqttClient);
  offset = writeEEPROMString(offset, mqttTopic);
  offset = writeEEPROMString(offset, channelID);
  offset = writeEEPROMString(offset, writeAPIKey);
  EEPROM.put(offset, postingInterval);
  offset += sizeof(postingInterval);
  EEPROM.put(offset, relayPin);
  offset += sizeof(relayPin);
  EEPROM.put(offset, relayLevel);
  offset += sizeof(relayLevel);
  EEPROM.put(offset, relayOnBoot);
  EEPROM.commit();
}
