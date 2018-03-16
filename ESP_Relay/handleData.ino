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
