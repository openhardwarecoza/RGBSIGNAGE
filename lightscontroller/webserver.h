void handleFileList() {
  
  String path = server.arg("dir");
  USE_SERIAL.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while(dir.next()){
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir)?"dir":"file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }
  
  output += "]";
  server.send(200, "text/json", output);
}

String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

void handle_aboutdevice() {
//  USE_SERIAL.println(server.uri());
  String qsid = server.arg("ssid");
  String qpass = server.arg("psk");
  if (qsid.length() > 0 && qpass.length() > 0) {
    for (int i = 0; i < qsid.length(); i++)
    {
      // Deal with (potentially) plus-encoded ssid
      qsid[i] = (qsid[i] == '+' ? ' ' : qsid[i]);
    }
    for (int i = 0; i < qpass.length(); i++)
    {
      // Deal with (potentially) plus-encoded password
      qpass[i] = (qpass[i] == '+' ? ' ' : qpass[i]);
    }

    strcpy(ssid, qsid.c_str());
    strcpy(passphrase, qpass.c_str());
    
    
  }
  String dawnmode = server.arg("dawndusk");
  if (dawnmode.length() > 0) {
    forceon = false;
  } else {
    forceon = true;
  }

if (!saveConfig()) {
      Serial.println("Failed to save config");
    } else {
      Serial.println("Config saved");
    }

  const uint8_t bufsize = 10; 

  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int Vcc = analogRead(A0); 
    
  char Up_time[bufsize]; 
  snprintf ( Up_time, bufsize, "%02d:%02d:%02d", hr, min % 60, sec % 60 );

  uint32_t _time = millis();

  String scanssids = "";
  int n = WiFi.scanNetworks();
  if (n > 0)
  {
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      if (WiFi.RSSI(i) < -90) {
        scanssids += "<div class=\"signal-bars mt1 sizing-box bad one-bar\">";
      } else if (WiFi.RSSI(i) < -80) {
        scanssids += "<div class=\"signal-bars mt1 sizing-box bad two-bars\">";
      } else if (WiFi.RSSI(i) < -70) {
        scanssids += "<div class=\"signal-bars mt1 sizing-box ok two-bars\">";
      } else if (WiFi.RSSI(i) < -67) {
       scanssids += "<div class=\"signal-bars mt1 sizing-box good three-bars\">";
      } else if (WiFi.RSSI(i) < -30) {
        scanssids += "<div class=\"signal-bars mt1 sizing-box good four-bars\">";
      }
      scanssids += "<div class=\"first-bar bar\"></div><div class=\"second-bar bar\"></div><div class=\"third-bar bar\"></div><div class=\"fourth-bar bar\"></div></div>";
      scanssids += "<a onclick=\"c(this)\" style=\"cursor: pointer;\">";
      scanssids += WiFi.SSID(i);
      scanssids += "</a>  ";
      scanssids += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"&#x1f512;";
      scanssids += "  ";
      scanssids += WiFi.RSSI(i);
      scanssids += "dbm<br><hr>";
    }
  }
// at the moment char arrays need to be created.. to cast to, especially for the ESP function returns.

  char heap[bufsize];
  char flashsize[bufsize];
  char flashsizeid[bufsize];
  char flashid[bufsize] ; 
  char chipid[bufsize] ; 
  char sketchsize[bufsize] ; 
  char freespace[bufsize] ; 
  char millisvar[bufsize] ; 
  char vcc[bufsize] ; 
  char rssi[bufsize] ; 
  char cpu[bufsize] ; 
  char wifiip[bufsize];
  char accesspointip[bufsize];
  char dawndusk[bufsize];
  
  strcpy(accesspointip, String(   WiFi.softAPIP().toString()       ).c_str()   );  
  strcpy(heap,        String(   ESP.getFreeHeap()).c_str() );
  strcpy(flashsize,   String(   ESP.getFlashChipSize()).c_str()   );
  strcpy(flashsizeid, String(   ESP.getFlashChipSizeByChipId()).c_str()   );
  strcpy(flashid, String(   ESP.getFlashChipId()               ).c_str()   );
  strcpy(chipid, String(   ESP.getChipId()      ).c_str()   );
  strcpy(sketchsize, String(   ESP.getSketchSize()  ).c_str()   );
  strcpy(freespace, String(   ESP.getFreeSketchSpace()  ).c_str()   );  
  strcpy(millisvar, String(   millis()      ).c_str()   );
  strcpy(vcc, String(   ESP.getVcc()         ).c_str()   ); 
  strcpy(rssi, String(   WiFi.RSSI()       ).c_str()   );
  strcpy(cpu, String(   ESP.getCpuFreqMHz()         ).c_str()   );
  strcpy(wifiip, String(   WiFi.localIP().toString()       ).c_str()   );

  if (forceon) {
    strcpy(dawndusk, String("").c_str()   );
  } else {
    strcpy(dawndusk, String("checked").c_str()   );
  }
  
  
  
     
   
  WiFiClient c = server.client();

  printer.BeginPage(c, SPIFFS, "/index.htm", 300);  // create the page, and sets aside memory for the variable pointers...  

  //  Add the variable.  Does not have to be in order of appearence... and it will replace all instances in target   use %_version_%  for example.. 
  printer.AddVariable(0,  "compile",    compile_date); 
  printer.AddVariable(1,  "heap",       heap  ); 
  printer.AddVariable(2,  "flashsize",  flashsize  );   
  printer.AddVariable(3,  "sketchsize", sketchsize  ); 
  printer.AddVariable(4,  "freespace",  freespace   ); 
  printer.AddVariable(5, "uptime" ,    Up_time);
  printer.AddVariable(6, "vcc",        vcc ); 
  printer.AddVariable(7, "rssi",       rssi   ); 
  printer.AddVariable(8, "cpu",        cpu   ); 
  printer.AddVariable(9, "ssid",        ssid);
  printer.AddVariable(10, "psk",        passphrase ); 
  printer.AddVariable(11, "availssids", scanssids.c_str()  ); 
  printer.AddVariable(12, "ssidname",   ssid   ); 
  printer.AddVariable(13, "clientip",   wifiip   ); 
  printer.AddVariable(14, "apip",       accesspointip   ); 
  printer.AddVariable(15, "apname",   softapname   ); 
  printer.AddVariable(16, "duskchecked",   dawndusk   ); 
  
   
  printer.SendPage();

  Serial.printf("Sent page %ums, heap = %u B\n", millis() - _time, ESP.getFreeHeap());


}


bool handleFileRead(String path){
  USE_SERIAL.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}
