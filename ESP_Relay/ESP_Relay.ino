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
const char* const timeArg PROGMEM = "timeMQTT";
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
const char* const writeAPIkey PROGMEM = "writeAPIKey";


uint32_t lastConnectionTime = 0; 
uint32_t postingInterval = 60L * 1000L; // Post data every 20 seconds.

String ssid, password, domain;
String mqttServer, mqttUser, mqttPassword, channelID, writeAPIKey, mqttClient = "ESP_TEMPERATURE", mqttTopic = "/175241/publish/EPY2NM6967MVDEM5", mqttTopicTempC = "/publish/EPY2NM6967MVDEM5";//, mqttTopicTempK = "/175241/publish/fields/field2/EPY2NM6967MVDEM5";
//String mqttServer, mqttUser, mqttPassword, mqttClient = "ESP_TEMPERATURE", mqttTopic = "/Relay", mqttTopicTempC = "/TempC", mqttTopicTempK = "/TempK";
uint16_t mqttPort = 1883;
byte relayPin = 5;
bool relayLevel = LOW;
bool relayOnBoot = false;

uint32_t times = 1635465468;
float tempK;                   // достаем температуру из структуры main
float tempC;

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient espClient;
PubSubClient pubsubClient(espClient);

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
