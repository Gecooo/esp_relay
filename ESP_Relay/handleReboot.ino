void handleReboot() {
  Serial.println(F("/reboot()"));

  String message =
F("<!DOCTYPE html>\n\
<html>\n\
<head>\n\
<meta charset=\"windows-1251\">\n\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n\
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
