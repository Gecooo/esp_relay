void handleRelaySwitch() {
  String on = httpServer.arg("on");

  Serial.print(F("/switch("));
  Serial.print(on);
  Serial.println(F(")"));

  switchRelay(on == "true");

  String message = "OK";
  httpServer.send(200, F("text/html"), message);
}
