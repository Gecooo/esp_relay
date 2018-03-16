void serialstring() {
  if (Serial.available()) {
    String inByte = Serial.readStringUntil('\n');
    StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.parseObject(inByte);
   
  String sensor = root["sensor"];             // достаем имя, 
 
  times = root["time"];
  //times = 123546879;
  tempK = root["data"][0];                   // достаем температуру из структуры main
  tempC = root["data"][1];                   // переводим кельвины в цельси

  if (mqttServer.length() && pubsubClient.connected()) {
    if (millis() - lastConnectionTime > postingInterval) 
  {
      String data = String("field1=" + String(tempC,0) + "&field2=" + String(tempK,0) + "&field3=" + times);
  int length = data.length();
  char msgBuffer[length];
  data.toCharArray(msgBuffer,length+1);
  //Serial.println(msgBuffer);
  
  // Create a topic string and publish data to ThingSpeak channel feed. 
  String topicString = "channels/" + channelID + "/publish/" + writeAPIKey;
  length=topicString.length();
  char topicBuffer[length];
  topicString.toCharArray(topicBuffer,length+1);
 
  
  pubsubClient.publish( topicBuffer, msgBuffer);      
  lastConnectionTime = millis();
        }
      }
    }
  }
