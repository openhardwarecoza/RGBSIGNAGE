String processor(const String& var)
{
  if(var == "_ssid_")
    return (ssid);
  if(var == "_psk_")
    return (password);
  if(var == "_compile_")
    return (compile_date);
  if(var == "_heap_")
    return (String(ESP.getFreeHeap()));
  if(var == "_flashsize_")
    return (String(   ESP.getFlashChipSize()));
  if(var == "_sketchsize_")
    return (String(ESP.getSketchSize()));
  if(var == "_freespace_")
    return (String(ESP.getFreeSketchSpace()));
  if(var == "_uptime_")
    return (String(millis()));
  if(var == "_rssi_")
    return (String(WiFi.RSSI()));
  if(var == "_cpu_")
    return (String(ESP.getCpuFreqMHz()));
  if(var == "_ssidname_")
    return (ssid);
  if(var == "_clientip_")
    return (String(WiFi.localIP().toString()));
  if(var == "_apip_")
    return (String(WiFi.softAPIP().toString()));
  if(var == "_apname_")
    return (hostName);   
  if(var == "_duskchecked_") {
    if (forceon) {
      return (String(""));
    } else {
      return (String("checked"));
    }
  }
   
  return String();
}

//  printer.AddVariable(2,  "flashsize",  flashsize  );   
//  printer.AddVariable(3,  "sketchsize", sketchsize  ); 
//  printer.AddVariable(4,  "freespace",  freespace   ); 


void setWebserver() {
  server.addHandler(&ws);

  events.onConnect([](AsyncEventSourceClient *client){
    client->send("hello!",NULL,millis(),1000);
  });
  server.addHandler(&events);

//  server.addHandler(new SPIFFSEditor(http_username,http_password));

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm").setTemplateProcessor(processor);

  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.printf("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      Serial.printf("GET");
    else if(request->method() == HTTP_POST)
      Serial.printf("POST");
    else if(request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if(request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if(request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if(request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if(request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });
  server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char*)data);
    if(final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  });
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!index)
      Serial.printf("BodyStart: %u\n", total);
    Serial.printf("%s", (const char*)data);
    if(index + len == total)
      Serial.printf("BodyEnd: %u\n", total);
  });
  server.begin();
  }
