void onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404);
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
  //Handle body
}

String processor(const String& var)

//  printer.AddVariable(0,  "compile",    compile_date); 
//  printer.AddVariable(1,  "heap",       heap  ); 
//  printer.AddVariable(2,  "flashsize",  flashsize  );   
//  printer.AddVariable(3,  "sketchsize", sketchsize  ); 
//  printer.AddVariable(4,  "freespace",  freespace   ); 
//  printer.AddVariable(5, "uptime" ,    Up_time);
//  printer.AddVariable(6, "vcc",        vcc ); 
//  printer.AddVariable(7, "rssi",       rssi   ); 
//  printer.AddVariable(8, "cpu",        cpu   ); 
//  printer.AddVariable(9, "ssid",        ssid);
//  printer.AddVariable(10, "psk",        passphrase ); 
//  printer.AddVariable(13, "availssids", scanssids.c_str()  ); 
//  printer.AddVariable(14, "ssidname",   ssid   ); 
//  printer.AddVariable(15, "clientip",   wifiip   ); 
//  printer.AddVariable(16, "apip",       accesspointip   ); 
//  printer.AddVariable(17, "apname",   softapname   ); 
//  printer.AddVariable(18, "duskchecked",   dawndusk   ); 

{

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
  if(var == "_compile_")
    return (compile_date);
  if(var == "_heap_")
    return (String(ESP.getFreeHeap()));
  if(var == "_flashsize_")
    return (String(   ESP.getFlashChipSize()));
  if(var == "_sketchsize_")
    return (String(   ESP.getSketchSize()  ));
  if(var == "_freespace_")
    return (String(   ESP.getFreeSketchSpace()  ));
  if(var == "_uptime_")
    return (String(   millis()      ));
  if(var == "_rssi_")
    return (String(   WiFi.RSSI()       ));
  if(var == "_cpu_")
    return (String(   ESP.getCpuFreqMHz()         ));
  if(var == "_ssid_")
    return (ssid);
  if(var == "_psk_")
    return (passphrase);
  if(var == "_availssids_")
    return (scanssids);
  if(var == "_ssidname_")
    return (ssid);
  if(var == "_clientip_")
    return (String(   WiFi.localIP().toString()       ));
  if(var == "_apip_")
    return (String(   WiFi.softAPIP().toString()       ));
  if(var == "_apname_")
    return (softapname);
    
  if(var == "_duskchecked_") {
    if (forceon) {
      return ("");
    } else {
      return ("checked");
    }
  }
  
    
  return String();
}

