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
