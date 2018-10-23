bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  json.prettyPrintTo(Serial);
  USE_SERIAL.printf("\n");

  strcpy(ssid, json["ssid"]);
  strcpy(passphrase, json["psk"]);
  sequence = json["sequence"];
  redval = json["red"];
  greenval = json["green"];
  blueval = json["blue"];
  analogWrite(12, redval);
  analogWrite(13, greenval);
  analogWrite(14, blueval);
  
  return true;
}

bool saveConfig() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["ssid"] = ssid;
  json["psk"] = passphrase;
  json["red"] = redval;
  json["green"] = greenval;
  json["blue"] = blueval;
  json["sequence"] = sequence;
  
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  json.prettyPrintTo(Serial);
  USE_SERIAL.printf("\n");

  return true;
}
