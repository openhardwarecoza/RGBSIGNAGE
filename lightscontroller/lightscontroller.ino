#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <HTTPPrinter.h>
#include <FS.h>
#include <ArduinoJson.h>

//  Uncomment to add Serial Prints - but comment for compile, too resource heavy
#define DEBUG

// Analog Light sensor
int lightthreshold = 500;
static const int MaxCount = 100;
int currentCount = 0;
long sum = 0;
int avg = 0;
bool forceon = true;

unsigned long previousMillis = 0; 
long interval = 1000; 
unsigned long wifiMillis = 0;
int sequence = 1;

WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);
HTTPPrinter printer; 
IPAddress ip;     
const char compile_date[] = __DATE__ " " __TIME__;
// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;
const char *myHostname = "sign";
char softapname[] = "SignSetup";
char softappsk[] = "karooheart";

String formatBytes(size_t bytes){
  if (bytes < 1024){
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+" MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}

#include "config.h" // default configs - some can be overwritten from spiffs config
#include "strings.h" // text strings / translate
#include "settings.h" // save/load settings from SPIFFS
#include "webserver.h" // onboard webserver
#include "websocket.h" // websocket for communication/feedback to/from the frontend webpages

/*  Working: 
 *  SPIFFS Webserver
 *  Websocket Server
 *  
 */



// Wifi Mode                    


void setup() {
    #ifdef DEBUG
      USE_SERIAL.begin(BAUD);
      USE_SERIAL.printf("\n\n\n\n");
    #endif

    delay(10);

    // SETUP GPIO
    #ifdef DEBUG
      USE_SERIAL.println("[1/9] Configuring GPIO...");
    #endif
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(14, OUTPUT);
    pinMode(0, INPUT);
    pinMode(2, INPUT);
    // LED OFF
    setColourRgb(0, 0, 0);
    delay(1);
    
    // MOUNT FILESYSTEM
    #ifdef DEBUG
      USE_SERIAL.println("[2/9] Mounting FS...");
    #endif
    if (!SPIFFS.begin()) {
      #ifdef DEBUG
        USE_SERIAL.println("Failed to mount file system");
      #endif
      return;
    }
    {
     Dir dir = SPIFFS.openDir("/");
      while (dir.next()) {    
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
        #ifdef DEBUG
          USE_SERIAL.printf("      FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
        #endif
      }
    }
    
    // LOAD CONFIG
    #ifdef DEBUG
      USE_SERIAL.println("[3/9] Loading configuration...");
    #endif
    if (!loadConfig()) {
      #ifdef DEBUG
        Serial.println("Failed to load config");
      #endif
    } else {
      #ifdef DEBUG
        Serial.println("Config loaded");
      #endif
    }

   // SETUP WIFI
    int wifiled = LOW;
    #ifdef DEBUG
      USE_SERIAL.println("[4/9] Setting up Wifi Client...");  
    #endif
  
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, passphrase);

    #ifdef DEBUG
      USE_SERIAL.print("       Waiting for Wifi...");
    #endif
    unsigned long startedWaiting = millis();
    int count = 0;
    while(WiFi.status() != WL_CONNECTED && millis() - startedWaiting <= 30000 ) {
        count++;
        if (count > 10) {
          #ifdef DEBUG
            USE_SERIAL.println("");
            USE_SERIAL.print("       Waiting for Wifi...");
          #endif
          count = 0;
        }
        if (wifiled == LOW) {
          setColourRgb(0, 0, 0);
        } else {
          setColourRgb(255, 255, 255);
        }
        
        delay(100);
        #ifdef DEBUG
          USE_SERIAL.print(".");    
        #endif
    }
    #ifdef DEBUG
      USE_SERIAL.println("");
    #endif
    if (WiFi.status() != WL_CONNECTED) {
      #ifdef DEBUG
       USE_SERIAL.println("       Failed to connect to the");
       USE_SERIAL.print("configured Wifi Network (");
       USE_SERIAL.print(ssid);
       USE_SERIAL.println(")");
      #endif
    } else {
      ip = WiFi.localIP();
      setColourRgb(255, 0, 0);
      #ifdef DEBUG
        USE_SERIAL.print("       Wifi Connected:  ");
        USE_SERIAL.print(ip);
        USE_SERIAL.print(" / ");
        USE_SERIAL.println(ssid);
      #endif
    }

    #ifdef DEBUG
      USE_SERIAL.println("[5/9] Setting up Wifi Access Point...");  
    #endif
    WiFi.softAP(softapname, softappsk);
    IPAddress myIP = WiFi.softAPIP();
    #ifdef DEBUG
      USE_SERIAL.println(myIP);
    #endif

    #ifdef DEBUG
      USE_SERIAL.println("[6/9] Setting up Captive Portal...");  
    #endif
     
    /* Setup the DNS server redirecting all the domains to the apIP */
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", myIP);

    #ifdef DEBUG
      USE_SERIAL.print("[7/9] Setting up mDNS for ");  
      USE_SERIAL.print(myHostname);  
      USE_SERIAL.println(".local");  
    #endif
    // Setup MDNS responder
    if (!MDNS.begin(myHostname)) {
      #ifdef DEBUG
        USE_SERIAL.println("       Error setting up MDNS responder!");
      #endif
    } else {
      #ifdef DEBUG
        USE_SERIAL.println("       mDNS responder started");
      #endif
      // Add service to MDNS-SD
      MDNS.addService("http", "tcp", 80);
    }

    // SETUP WEBSOCKET
    #ifdef DEBUG
      USE_SERIAL.println("[8/9] Setting up WebSocket...");
    #endif
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

   // SETUP SPIFFS WEBSERVER
   #ifdef DEBUG
   USE_SERIAL.println("[9/9] Starting Webserver...");
   #endif
   //called when the url is not defined here
   //use it to load content from SPIFFS
   server.on("/", handle_aboutdevice);
   server.on("/generate_204", handle_aboutdevice);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
   server.on("/fwlink", handle_aboutdevice);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
   server.onNotFound([](){
      if(!handleFileRead(server.uri()))
        server.send(404, "text/plain", "FileNotFound");
    });
    server.begin();

    // End Boot process: Ready for Action
    
    USE_SERIAL.println("");
    USE_SERIAL.println("BOOT COMPLETE");

}

void housekeeping() {
  dnsServer.processNextRequest();
  webSocket.loop();
  server.handleClient();
  
  if(currentCount < MaxCount) {
    currentCount++;
    sum += analogRead(A0);
  } else {
    avg = sum / MaxCount;
    currentCount = 0;
    sum = 0;   
  } 
}

void loop() {
      
  // USE_SERIAL.print("Seq" );
  // USE_SERIAL.println(sequence);

  bool btn2 = digitalRead(pin_btn2); 
  if (!btn2) {
    sequence++;
    delay(200);
    if (sequence == 9 || sequence > 9 ) {
      sequence = 1;
    }
    saveConfig();
  } 
  
  bool btn0 = digitalRead(pin_btn0); 
  if (!btn0) {
    forceon = !forceon;
    setColourRgb(1024, 1024, 1024);
    delay(200);  
    setColourRgb(0, 0, 0);
    saveConfig();
  } 
  
  housekeeping();

  if (forceon) { // then always turn on
    runProgram(sequence);  
  } else if (avg < lightthreshold) { // if its not in forceon mode, we check, is it dark? then turn on
    runProgram(sequence);  
  } else { // Not dark yet, so turn off.  Or its just turned morning. Off again. 
    setColourRgb(0, 0, 0);
  }
  housekeeping();     
  delay(1);
}

void setColourRgb(unsigned int red, unsigned int green, unsigned int blue) {
  analogWrite(12, red);
  analogWrite(13, green);
  analogWrite(14, blue);
 }

void runProgram(unsigned int id) {
  switch (id) {
    case 1:
      setColourRgb(1024, 0, 0);
      break;
    case 2:
      setColourRgb(1024, 1024, 0);
      break;
    case 3:
      setColourRgb(0, 1024, 0);
      break;
    case 4:
      setColourRgb(0, 1024, 1024);
      break;
    case 5:
      setColourRgb(0, 0, 1024);
      break;
    case 6: 
      setColourRgb(1024, 0, 1024);
      break;
    case 7:  // Pulsing Heartbeat
      setColourRgb(1024, 0, 0);
      if (digitalRead(pin_btn2) == 0 || digitalRead(pin_btn0) == 0) {
          break;
        }
      housekeeping();
      delay(200);
      setColourRgb(100, 0, 0);
      if (digitalRead(pin_btn2) == 0 || digitalRead(pin_btn0) == 0) {
          break;
        }
      housekeeping();
      delay(200);
      setColourRgb(1024, 0, 0);
      if (digitalRead(pin_btn2) == 0 || digitalRead(pin_btn0) == 0) {
          break;
        }
      housekeeping();
      delay(200);
      for(int fadeValue = 1024 ; fadeValue >= 100; fadeValue -=5){   ///fades the LED
        setColourRgb(fadeValue, 0, 0);
        if (digitalRead(pin_btn2) == 0 || digitalRead(pin_btn0) == 0) {
          break;
        }
        housekeeping();
        delay(3);
      }
      break;
    case 8: // Rainbow Cycle
      unsigned int rgbColour[3];
      rgbColour[0] = 255;
      rgbColour[1] = 0;
      rgbColour[2] = 0;  

      // Choose the colours to increment and decrement.
      for (int decColour = 0; decColour < 3; decColour += 1) {
        int incColour = decColour == 2 ? 0 : decColour + 1;

        // cross-fade the two colours.
        for(int i = 0; i < 255; i += 1) {
          rgbColour[decColour] -= 1;
          rgbColour[incColour] += 1;
          setColourRgb(rgbColour[0]*4, rgbColour[1]*4, rgbColour[2]*4);
          if (digitalRead(pin_btn2) == 0 || digitalRead(pin_btn0) == 0) {
            break;
          }
          housekeeping();
          delay(5);
        }
      }
      default: // Fallback to all-on
        // do nothing
        break;   
    }
}

