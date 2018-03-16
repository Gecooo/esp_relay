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
    <br/>\n\
    Thingspeak Channel Write API Key:<br/>\n\
    <input type=\"password\" name=\"");
  message += FPSTR(writeAPIkey);
  message += F("\" maxlength=");
  message += String(maxStrParamLength);
  message += F(" value=\"");
  message += quoteEscape(writeAPIKey);
  message += F("\" />\n\
  <br/>\n\
    Интервал передачи данных(мсек):<br/>\n\
    <input type=\"text\" name=\"");
  message += FPSTR(timeArg);
  message += F("\" maxlength=6 value=\"");
  message += String(postingInterval);
  message += F("\" onchange=\"document.mqtt.reboot.value=1;\" />\n\
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
