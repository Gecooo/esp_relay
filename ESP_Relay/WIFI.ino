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

