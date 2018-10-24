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
  Serial.printf("\n");

//  ssid =  json["ssid"];
//  password = json["psk"];
  sequence = json["sequence"];
  redval = json["red"];
  greenval = json["green"];
  blueval = json["blue"];
  forceon = json["forceon"];
  analogWrite(12, redval);
  analogWrite(13, greenval);
  analogWrite(14, blueval);
  
  return true;
}

bool saveConfig() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["ssid"] = ssid;
  json["psk"] = password;
  json["red"] = redval;
  json["green"] = greenval;
  json["blue"] = blueval;
  json["sequence"] = sequence;
  json["forceon"] = forceon;
  
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  json.prettyPrintTo(Serial);
  Serial.printf("\n");

  return true;
}

void parseParams(String param, String value) {
  Serial.print("param:");
  Serial.print(param);
  Serial.print(" / Value:");
  Serial.println(value);
  if (param == "dawndusk") {
    if (value == "true") {
      forceon = false;
    } else {
      forceon = true;
    }  
  }

  if (!saveConfig()) {
    Serial.println("Failed to save config");
  } else {
    Serial.println("Config saved");
  }

}
