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
    <input type=\"button\" value=\"Reboot!\" onclick=\"if (confirm('Вы уверены, что хотите перезагрузить?')) location.href='/reboot';\" />\n\
    <input type=\"button\" value=\"Update\" onclick=\"location.href='/update';\" />\n\
  </form>\n\
</body>\n\
</html>");

  httpServer.send(200, F("text/html"), message);
}
