void handleRelayConfig() {
  const byte gpios[] = { 0, 1, 2, 3, 4, 5, 12, 13, 14, 15, 16 };

  String message =
F("<!DOCTYPE html>\n\
<html>\n\
<head>\n\
<meta charset=\"windows-1251\">\n\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n\
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
