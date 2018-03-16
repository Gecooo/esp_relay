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
      } else if (argName.equals(FPSTR(timeArg))) {
      postingInterval = argValue.toInt();
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
      } else if (argName.equals(FPSTR(writeAPIkey))) {
      writeAPIKey = argValue;
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
  Конфигурация сохранена успешно.\n");
  if (httpServer.arg(rebootArg) == "1")
    message += F("  <br/>\n\
  <i>Вы должны перезагрузить модуль, чтобы применить новую конфигурацию!</i>\n");
  message += F("  <p>\n\
  Ожидайте 5 sec. или нажмите <a href=\"/index.html\">здесь</a> чтобы вернуться на главную страницу.\n\
</body>\n\
</html>");

  httpServer.send(200, F("text/html"), message);
}
