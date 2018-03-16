#include <pgmspace.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

#define CHARSET "windows-1251"

ADC_MODE(ADC_VCC);

const char* const ssidAP PROGMEM = "ESP_CONTROL";
const char* const passwordAP PROGMEM = "Pa$$w0rd";

const byte pinBuiltinLed = 2; // Change BLED GPIO according to your board

const char configSign[4] PROGMEM = { '#', 'R', 'E', 'L' };
const byte maxStrParamLength = 32;

const char* const ssidArg PROGMEM = "ssid";
const char* const passwordArg PROGMEM = "password";
const char* const domainArg PROGMEM = "domain";
const char* const serverArg PROGMEM = "server";
const char* const portArg PROGMEM = "port";
const char* const userArg PROGMEM = "user";
const char* const mqttpswdArg PROGMEM = "mqttpswd";
const char* const clientArg PROGMEM = "client";
const char* const topicArg PROGMEM = "topic";
const char* const gpioArg PROGMEM = "gpio";
const char* const levelArg PROGMEM = "level";
const char* const onbootArg PROGMEM = "onboot";
const char* const rebootArg PROGMEM = "reboot";
const char* const vccArg PROGMEM = "vcc";
const char* const tempArg PROGMEM = "tempC";
const char* const tempKArg PROGMEM = "tempK";
const char* const wifimodeArg PROGMEM = "wifimode";
const char* const mqttconnectedArg PROGMEM = "mqttconnected";
const char* const freeheapArg PROGMEM = "freeheap";
const char* const uptimeArg PROGMEM = "uptime";
const char* const relayArg PROGMEM = "relay";
const char* const channelId PROGMEM = "channelID";


unsigned long lastConnectionTime = 0; 
const unsigned long postingInterval = 20L * 1000L; // Post data every 20 seconds.

String ssid, password, domain;
String mqttServer, mqttUser, mqttPassword, channelID, mqttClient = "ESP_TEMPERATURE", mqttTopic = "/175241/publish/EPY2NM6967MVDEM5", mqttTopicTempC = "/publish/EPY2NM6967MVDEM5";//, mqttTopicTempK = "/175241/publish/fields/field2/EPY2NM6967MVDEM5";
//String mqttServer, mqttUser, mqttPassword, mqttClient = "ESP_TEMPERATURE", mqttTopic = "/Relay", mqttTopicTempC = "/TempC", mqttTopicTempK = "/TempK";
uint16_t mqttPort = 1883;
byte relayPin = 5;
bool relayLevel = LOW;
bool relayOnBoot = false;

long times;
float tempK;                   // достаем температуру из структуры main
float tempC;

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient espClient;
PubSubClient pubsubClient(espClient);

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
  EEPROM.put(offset, relayPin);
  offset += sizeof(relayPin);
  EEPROM.put(offset, relayLevel);
  offset += sizeof(relayLevel);
  EEPROM.put(offset, relayOnBoot);
  EEPROM.commit();
}

/*
 * WiFi setup functions
 */

bool setupWiFiAsStation() {
  const uint32_t timeout = 60000;
  uint32_t maxtime = millis() + timeout;

  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(pinBuiltinLed, LOW);
    delay(500);
    digitalWrite(pinBuiltinLed, HIGH);
    Serial.print(".");
    if (millis() >= maxtime) {
      Serial.println(F(" fail!"));

      return false;
    }
  }
  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  return true;
}

void setupWiFiAsAP() {
  Serial.print(F("Configuring access point "));
  Serial.println(ssidAP);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssidAP, passwordAP);

  Serial.print(F("IP address: "));
  Serial.println(WiFi.softAPIP());
}

void setupWiFi() {
  if ((! ssid.length()) || (! setupWiFiAsStation()))
    setupWiFiAsAP();

  if (domain.length()) {
    if (MDNS.begin(domain.c_str())) {
      MDNS.addService("http", "tcp", 80);
      Serial.println(F("mDNS responder started"));
    } else {
      Serial.println(F("Error setting up mDNS responder!"));
    }
  }

  httpServer.begin();
  Serial.println(F("HTTP server started (use '/update' url to OTA update)"));
}

/*
 * HTTP server functions
 */

String quoteEscape(const String& str) {
  String result = "";
  int start = 0, pos;

  while (start < str.length()) {
    pos = str.indexOf('"', start);
    if (pos != -1) {
      result += str.substring(start, pos) + F("&quot;");
      start = pos + 1;
    } else {
      result += str.substring(start);
      break;
    }
  }

  return result;
}

void handleRoot() {
  String message =
F("<!DOCTYPE html>\n\
<html>\n\
<head>\n\
<meta charset=\""CHARSET"\">\n\
  <title>ESP Relay</title>\n\
   <style type=\"text/css\">\n\
    .checkbox {\n\
      vertical-align:top;\n\
      margin:0 3px 0 0;\n\
      width:17px;\n\
      height:17px;\n\
    }\n\
    .checkbox + label {\n\
      cursor:pointer;\n\
    }\n\
    .checkbox:not(checked) {\n\
      position:absolute;\n\
      opacity:0;\n\
    }\n\
    .checkbox:not(checked) + label {\n\
      position:relative;\n\
      padding:0 0 0 60px;\n\
    }\n\
    .checkbox:not(checked) + label:before {\n\
      content:'';\n\
      position:absolute;\n\
      top:-4px;\n\
      left:0;\n\
      width:50px;\n\
      height:26px;\n\
      border-radius:13px;\n\
      background:#CDD1DA;\n\
      box-shadow:inset 0 2px 3px rgba(0,0,0,.2);\n\
    }\n\
    .checkbox:not(checked) + label:after {\n\
      content:'';\n\
      position:absolute;\n\
      top:-2px;\n\
      left:2px;\n\
      width:22px;\n\
      height:22px;\n\
      border-radius:10px;\n\
      background:#FFF;\n\
      box-shadow:0 2px 5px rgba(0,0,0,.3);\n\
      transition:all .2s;\n\
    }\n\
    .checkbox:checked + label:before {\n\
      background:#9FD468;\n\
    }\n\
    .checkbox:checked + label:after {\n\
      left:26px;\n\
    }\n\
  </style>\n\
  <script type=\"text/javascript\">\n\
    function openUrl(url) {\n\
      var request = new XMLHttpRequest();\n\
      request.open('GET', url, true);\n\
      request.send(null);\n\
    }\n\
    function refreshData() {\n\
      var request = new XMLHttpRequest();\n\
      request.open('GET', '/data', true);\n\
      request.onreadystatechange = function() {\n\
        if (request.readyState == 4) {\n\
          var data = JSON.parse(request.responseText);\n\
          document.getElementById('");
  message += FPSTR(vccArg);
  message += F("').innerHTML = data.");
  message += FPSTR(vccArg);
  message += F(";\n\
          document.getElementById('");
  message += FPSTR(wifimodeArg);
  message += F("').innerHTML = data.");
  message += FPSTR(wifimodeArg);
  message += F(";\n\
          document.getElementById('");
  message += FPSTR(mqttconnectedArg);
  message += F("').innerHTML = (data.");
  message += FPSTR(mqttconnectedArg);
  message += F(" != true ? \"не \" : \"\") + \"подключен\";\n\
          document.getElementById('");
  message += FPSTR(freeheapArg);
  message += F("').innerHTML = data.");
  message += FPSTR(freeheapArg);
  message += F(";\n\
          document.getElementById('");
  message += FPSTR(uptimeArg);
  message += F("').innerHTML = data.");
  message += FPSTR(uptimeArg);
  message += F(";\n\
          document.getElementById('");
  message += FPSTR(relayArg);
  message += F("').checked = data.");
  message += FPSTR(relayArg);
  message += F(";\n\
    document.getElementById('");
  message += FPSTR(tempArg);
  message += F("').innerHTML = data.");
  message += FPSTR(tempArg);
  message += F(";\n\
     document.getElementById('");
  message += FPSTR(tempKArg);
  message += F("').innerHTML = data.");
  message += FPSTR(tempKArg);
  message += F(";\n\
        }\n\
      }\n\
      request.send(null);\n\
    }\n\
    setInterval(refreshData, 500);\n\
  </script>\n\
</head>\n\
<body>\n\
  <form>\n\
    <h3>ESP Relay</h3>\n\
    <p>\n\
    VCC: <span id=\"");
  message += FPSTR(vccArg);
  message += F("\">?</span> V<br/>\n\
    WiFi mode: <span id=\"");
  message += FPSTR(wifimodeArg);
  message += F("\">?</span><br/>\n\
    MQTT broker: <span id=\"");
  message += FPSTR(mqttconnectedArg);
  message += F("\">?</span><br/>\n\
    Свободная память: <span id=\"");
  message += FPSTR(freeheapArg);
  message += F("\">0</span> bytes<br/>\n\
   Время работы: <span id=\"");
  message += FPSTR(uptimeArg);
  message += F("\">0</span> секунд<br/>\n\
    Температруа: <span id=\"");
  message += FPSTR(tempArg);
 // message += (tempC);
  message += F("\">0</span> цельсий<br/>\n\ 
    Температруа: <span id=\"");
  message += FPSTR(tempKArg);
 // message += (tempC);
  message += F("\">0</span> кельвин</p>\n\ 
    <input type=\"checkbox\" class=\"checkbox\" id=\"");
  message += FPSTR(relayArg);
  message += F("\" onchange=\"openUrl('/switch?on=' + this.checked);\" ");
  if (digitalRead(relayPin) == relayLevel)
    message += "checked ";
  message += F("/>\n\
    <label for=\"");
  message += FPSTR(relayArg);
  message += F("\">Relay</label>\n\
    <p>\n\
    <input type=\"button\" value=\"WiFi Setup\" onclick=\"location.href='/wifi';\" />\n\
    <input type=\"button\" value=\"MQTT Setup\" onclick=\"location.href='/mqtt';\" />\n\
    <input type=\"button\" value=\"Relay Setup\" onclick=\"location.href='/relay';\" />\n\
    <input type=\"button\" value=\"Reboot!\" onclick=\"if (confirm('Are you sure to reboot?')) location.href='/reboot';\" />\n\
  </form>\n\
</body>\n\
</html>");

  httpServer.send(200, F("text/html"), message);
}

void handleWiFiConfig() {
  String message =
F("<!DOCTYPE html>\n\
<html>\n\
<head>\n\
  <title>WiFi Setup</title>\n\
</head>\n\
<body>\n\
  <form name=\"wifi\" method=\"get\" action=\"/store\">\n\
    <h3>WiFi Setup</h3>\n\
    SSID:<br/>\n\
    <input type=\"text\" name=\"");
  message += FPSTR(ssidArg);
  message += F("\" maxlength=");
  message += String(maxStrParamLength);
  message += F(" value=\"");
  message += quoteEscape(ssid);
  message += F("\" />\n\
    <br/>\n\
    Password:<br/>\n\
    <input type=\"password\" name=\"");
  message += FPSTR(passwordArg);
  message += F("\" maxlength=");
  message += String(maxStrParamLength);
  message += F(" value=\"");
  message += quoteEscape(password);
  message += F("\" />\n\
    <br/>\n\
    mDNS domain:<br/>\n\
    <input type=\"text\" name=\"");
  message += FPSTR(domainArg);
  message += F("\" maxlength=");
  message += String(maxStrParamLength);
  message += F(" value=\"");
  message += quoteEscape(domain);
  message += F("\" />\n\
    .local (leave blank to ignore mDNS)\n\
    <p>\n\
    <input type=\"submit\" value=\"Save\" />\n\
    <input type=\"hidden\" name=\"");
  message += FPSTR(rebootArg);
  message += F("\" value=\"1\" />\n\
  </form>\n\
</body>\n\
</html>");

  httpServer.send(200, F("text/html"), message);
}

void handleMQTTConfig() {
  String message =
F("<!DOCTYPE html>\n\
<html>\n\
<head>\n\
  <title>MQTT Setup</title>\n\
</head>\n\
<body>\n\
  <form name=\"mqtt\" method=\"get\" action=\"/store\">\n\
    <h3>MQTT Setup</h3>\n\
    Server:<br/>\n\
    <input type=\"text\" name=\"");
  message += FPSTR(serverArg);
  message += F("\" maxlength=");
  message += String(maxStrParamLength);
  message += F(" value=\"");
  message += quoteEscape(mqttServer);
  message += F("\" onchange=\"document.mqtt.reboot.value=1;\" />\n\
    (leave blank to ignore MQTT)\n\
    <br/>\n\
    Port:<br/>\n\
    <input type=\"text\" name=\"");
  message += FPSTR(portArg);
  message += F("\" maxlength=5 value=\"");
  message += String(mqttPort);
  message += F("\" onchange=\"document.mqtt.reboot.value=1;\" />\n\
    <br/>\n\
    User (if authorization is required on MQTT server):<br/>\n\
    <input type=\"text\" name=\"");
  message += FPSTR(userArg);
  message += F("\" maxlength=");
  message += String(maxStrParamLength);
  message += F(" value=\"");
  message += quoteEscape(mqttUser);
  message += F("\" />\n\
    (leave blank to ignore MQTT authorization)\n\
    <br/>\n\
    Password:<br/>\n\
    <input type=\"password\" name=\"");
  message += FPSTR(mqttpswdArg);
  message += F("\" maxlength=");
  message += String(maxStrParamLength);
  message += F(" value=\"");
  message += quoteEscape(mqttPassword);
  message += F("\" />\n\
    <br/>\n\
    Client:<br/>\n\
    <input type=\"text\" name=\"");
  message += FPSTR(clientArg);
  message += F("\" maxlength=");
  message += String(maxStrParamLength);
  message += F(" value=\"");
  message += quoteEscape(mqttClient);
  message += F("\" />\n\
    <br/>\n\
    Topic:<br/>\n\
    <input type=\"text\" name=\"");
  message += FPSTR(topicArg);
  message += F("\" maxlength=");
  message += String(maxStrParamLength);
  message += F(" value=\"");
  message += quoteEscape(mqttTopic);
  message += F("\" />\n\
    <br/>\n\
    Thingspeak Channel ID:<br/>\n\
    <input type=\"text\" name=\"");
  message += FPSTR(channelId);
  message += F("\" maxlength=");
  message += String(maxStrParamLength);
  message += F(" value=\"");
  message += quoteEscape(channelID);
  message += F("\" />\n\
    <p>\n\
    <input type=\"submit\" value=\"Save\" />\n\
    <input type=\"hidden\" name=\"");
  message += FPSTR(rebootArg);
  message += F("\" value=\"0\" />\n\
  </form>\n\
</body>\n\
</html>");

  httpServer.send(200, F("text/html"), message);
}

void handleRelayConfig() {
  const byte gpios[] = { 0, 1, 2, 3, 4, 5, 12, 13, 14, 15, 16 };

  String message =
F("<!DOCTYPE html>\n\
<html>\n\
<head>\n\
  <title>Relay Setup</title>\n\
</head>\n\
<body>\n\
  <form name=\"relay\" method=\"get\" action=\"/store\">\n\
    <h3>Relay Setup</h3>\n\
    GPIO:<br/>\n\
    <select name=\"");
  message += FPSTR(gpioArg);
  message += F("\" size=1>\n");
  for (byte i = 0; i < sizeof(gpios); i++) {
    message += F("      <option value=\"");
    message += String(gpios[i]);
    message += '\"';
    if (relayPin == gpios[i])
      message += F(" selected");
    message += '>';
    message += String(gpios[i]);
    message += F("</option>\n");
  }
  message += F("    </select>\n\
    <br/>\n\
    Logical level to switch:<br/>\n\
    <input type=\"radio\" name=\"");
  message += FPSTR(levelArg);
  message += F("\" value=\"1\" ");
  if (relayLevel)
    message += F("checked ");
  message += F("/>HIGH\n\
    <input type=\"radio\" name=\"");
  message += FPSTR(levelArg);
  message += F("\" value=\"0\" ");
  if (! relayLevel)
    message += F("checked ");
  message += F("/>LOW\n\
    <br/>\n\
    State on boot:<br/>\n\
    <input type=\"radio\" name=\"");
  message += FPSTR(onbootArg);
  message += F("\" value=\"1\" ");
  if (relayOnBoot)
    message += F("checked ");
  message += F("/>On\n\
    <input type=\"radio\" name=\"");
  message += FPSTR(onbootArg);
  message += F("\" value=\"0\" ");
  if (! relayOnBoot)
    message += F("checked ");
  message += F("/>Off\n\
    <p>\n\
    <input type=\"submit\" value=\"Save\" />\n\
    <input type=\"hidden\" name=\"");
  message += FPSTR(rebootArg);
  message += F("\" value=\"1\" />\n\
  </form>\n\
</body>\n\
</html>");

  httpServer.send(200, F("text/html"), message);
}

void handleStoreConfig() {
  String argName, argValue;

  Serial.print(F("/store("));
  for (byte i = 0; i < httpServer.args(); i++) {
    if (i)
      Serial.print(F(", "));
    argName = httpServer.argName(i);
    Serial.print(argName);
    Serial.print(F("=\""));
    argValue = httpServer.arg(i);
    Serial.print(argValue);
    Serial.print(F("\""));

    if (argName.equals(FPSTR(ssidArg))) {
      ssid = argValue;
    } else if (argName.equals(FPSTR(passwordArg))) {
      password = argValue;
    } else if (argName.equals(FPSTR(domainArg))) {
      domain = argValue;
    } else if (argName.equals(FPSTR(serverArg))) {
      mqttServer = argValue;
    } else if (argName.equals(FPSTR(portArg))) {
      mqttPort = argValue.toInt();
    } else if (argName.equals(FPSTR(userArg))) {
      mqttUser = argValue;
    } else if (argName.equals(FPSTR(mqttpswdArg))) {
      mqttPassword = argValue;
    } else if (argName.equals(FPSTR(clientArg))) {
      mqttClient = argValue;
    } else if (argName.equals(FPSTR(topicArg))) {
      mqttTopic = argValue;
      } else if (argName.equals(FPSTR(channelId))) {
      channelID = argValue;
    } else if (argName.equals(FPSTR(gpioArg))) {
      relayPin = argValue.toInt();
    } else if (argName.equals(FPSTR(levelArg))) {
      relayLevel = argValue.toInt();
    } else if (argName.equals(FPSTR(onbootArg))) {
      relayOnBoot = argValue.toInt();
    }
  }
  Serial.println(F(")"));

  writeConfig();

  String message =
F("<!DOCTYPE html>\n\
<html>\n\
<head>\n\
  <title>Store Setup</title>\n\
  <meta http-equiv=\"refresh\" content=\"5; /index.html\">\n\
</head>\n\
<body>\n\
  Configuration stored successfully.\n");
  if (httpServer.arg(rebootArg) == "1")
    message += F("  <br/>\n\
  <i>You must reboot module to apply new configuration!</i>\n");
  message += F("  <p>\n\
  Wait for 5 sec. or click <a href=\"/index.html\">this</a> to return to main page.\n\
</body>\n\
</html>");

  httpServer.send(200, F("text/html"), message);
}

void handleRelaySwitch() {
  String on = httpServer.arg("on");

  Serial.print(F("/switch("));
  Serial.print(on);
  Serial.println(F(")"));

  switchRelay(on == "true");

  String message = "OK";
  httpServer.send(200, F("text/html"), message);
}

void handleReboot() {
  Serial.println(F("/reboot()"));

  String message =
F("<!DOCTYPE html>\n\
<html>\n\
<head>\n\
  <title>Rebooting</title>\n\
  <meta http-equiv=\"refresh\" content=\"5; /index.html\">\n\
</head>\n\
<body>\n\
  Rebooting...\n\
</body>\n\
</html>");

  httpServer.send(200, F("text/html"), message);

  ESP.restart();
}

void handleData() {
  String message = F("{\"");
  message += FPSTR(vccArg);
  message += F("\":");
  message += String(ESP.getVcc() / 1024.0);
  message += F(",\"");
  message += FPSTR(wifimodeArg);
  message += F("\":\"");
  switch (WiFi.getMode()) {
    case WIFI_OFF:
      message += F("OFF");
      break;
    case WIFI_STA:
      message += F("Station");
      break;
    case WIFI_AP:
      message += F("Access Point");
      break;
    case WIFI_AP_STA:
      message += F("Hybrid (AP+STA)");
      break;
    default:
      message += F("Unknown!");
  }
  message += F("\",\"");
  message += FPSTR(mqttconnectedArg);
  message += F("\":");
  if (pubsubClient.connected())
    message += F("true");
  else
    message += F("false");
  message += F(",\"");
  message += FPSTR(freeheapArg);
  message += F("\":");
  message += String(ESP.getFreeHeap());
  message += F(",\"");
  message += FPSTR(uptimeArg);
  message += F("\":");
  message += String(millis() / 1000);
  message += F(",\"");
  message += FPSTR(tempArg);
  message += F("\":");
  message += String(tempC);
  message += F(",\"");
  message += FPSTR(tempKArg);
  message += F("\":");
  message += String(tempK);
  message += F(",\"");
  message += FPSTR(relayArg);
  message += F("\":");
  if (digitalRead(relayPin) == relayLevel)
    message += F("true");
  else
    message += F("false");
  message += F("}");

  httpServer.send(200, F("text/html"), message);
}

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

void switchRelay(bool on) {
  bool relay = digitalRead(relayPin);

  if (! relayLevel)
    relay = ! relay;
  if (relay != on) {
    digitalWrite(relayPin, relayLevel == on);

    if (mqttServer.length() && pubsubClient.connected()) {
      String topic('/');
      topic += mqttClient;
      topic += mqttTopic;
      mqtt_publish(pubsubClient, topic, String(on));
    }
  }
}

void serialstring() {
//  const uint32_t timeout1 = 300000;
//  static uint32_t lastTime1;
  
  if (Serial.available()) {
    String inByte = Serial.readStringUntil('\n');
    StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.parseObject(inByte);
   
  String sensor = root["sensor"];                           // достаем имя, 
 
  times = root["time"];
  tempK = root["data"][0];                   // достаем температуру из структуры main
  tempC = root["data"][1];                         // переводим кельвины в цельси

  if (mqttServer.length() && pubsubClient.connected()) {
    if (millis() - lastConnectionTime > postingInterval) 
  {
      String data = String("field1=" + String(tempC,0) + "&field2=" + String(tempK,0));
  int length = data.length();
  char msgBuffer[length];
  data.toCharArray(msgBuffer,length+1);
  //Serial.println(msgBuffer);
  
  // Create a topic string and publish data to ThingSpeak channel feed. 
  String topicString = "channels/" + channelID + mqttTopicTempC;
//  topicString += mqttClient;
//  topicString += mqttTopic;
  length=topicString.length();
  char topicBuffer[length];
  topicString.toCharArray(topicBuffer,length+1);
 
  //mqttClient.publish( topicBuffer, msgBuffer );
  pubsubClient.publish( topicBuffer, msgBuffer);      
  lastConnectionTime = millis();
        }
      }
    }
  }

//void mqttpublish() {
  
//      String topic;//('/');
//      topic += mqttClient;
//      topic += mqttTopicTempC;
//      mqtt_publish(pubsubClient, topic, String(tempC));
//
//     // topic = '/';
//      topic = mqttClient;
//      topic += mqttTopicTempK;
//      mqtt_publish(pubsubClient, topic, String(tempK));
// Create data string to send to ThingSpeak

//}
/*
 * Main setup
 */

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(pinBuiltinLed, OUTPUT);

  EEPROM.begin(1024);
  if (! readConfig())
    Serial.println(F("EEPROM is empty!"));

  digitalWrite(relayPin, relayLevel == relayOnBoot);
  pinMode(relayPin, OUTPUT);

  setupWiFi();

  httpUpdater.setup(&httpServer);
  httpServer.onNotFound([]() {
    httpServer.send(404, F("text/plain"), F("FileNotFound"));
  });
  httpServer.on("/", handleRoot);
  httpServer.on("/index.html", handleRoot);
  httpServer.on("/wifi", handleWiFiConfig);
  httpServer.on("/mqtt", handleMQTTConfig);
  httpServer.on("/relay", handleRelayConfig);
  httpServer.on("/store", handleStoreConfig);
  httpServer.on("/switch", handleRelaySwitch);
  httpServer.on("/reboot", handleReboot);
  httpServer.on("/data", handleData);

  if (mqttServer.length()) {
    pubsubClient.setServer(mqttServer.c_str(), mqttPort);
    pubsubClient.setCallback(mqttCallback);
  }
}

/*
 * Main loop
 */

void loop() {
  if ((WiFi.getMode() == WIFI_STA) && (WiFi.status() != WL_CONNECTED)) {
    setupWiFi();
  }

  httpServer.handleClient();

  if (mqttServer.length() && (WiFi.getMode() == WIFI_STA)) {
    if (! pubsubClient.connected())
      mqttReconnect();
    if (pubsubClient.connected())
      pubsubClient.loop();
  }
serialstring();
  delay(1); // For WiFi maintenance
}
