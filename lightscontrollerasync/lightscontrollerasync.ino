#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

//  Uncomment to add Serial Prints - but comment for compile, too resource heavy
#define DEBUG

// SKETCH BEGIN
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

char* ssid = "OPS_DSL";
char* password = "ffeeddccbbaa";
const char * hostName = "SignSetup(async)";
const char compile_date[] = __DATE__ " " __TIME__;
int pin_btn0 = 0;
int pin_btn2 = 2;
int redval = 0;
int greenval = 0;
int blueval = 0;
int sequence = 1;
bool forceon = true;
String scanssids = "";
unsigned long previousScanMillis = 0;
const long scanInterval = 10000;

#include "settings.h" // Save/Load settings from SPIFFS
#include "rgb.h" // RGB LED controller
#include "webserver.h" // onboard webserver
#include "websocket.h" // websocket for communication/feedback to/from the frontend webpages

void setup(){
  #ifdef DEBUG
    Serial.println("[1/9] Starting Serial...");
  #endif
  Serial.begin(115200);
  //Serial.setDebugOutput(true);

  #ifdef DEBUG
    Serial.println("[2/9] Starting Wifi Client...");
  #endif
  WiFi.hostname(hostName);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(ssid, password);
  }
  
  #ifdef DEBUG
    Serial.println("[3/9] Starting Wifi AP...");
  #endif
  WiFi.softAP(hostName);

  #ifdef DEBUG
    Serial.println("[4/9] Starting mDNS...");
  #endif
  MDNS.addService("http","tcp",80);

  #ifdef DEBUG
    Serial.println("[4/9] Starting SPIFFS...");
  #endif
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  if (ideSize != realSize) {
    Serial.println("      Flash Chip configuration wrong!");
    Serial.printf( "      Flash real size: %u bytes\n", realSize);
    Serial.printf( "      Flash ide  size: %u bytes\n", ideSize);
  } else {
    Serial.println("      Flash Chip configuration ok.");
    Serial.printf( "      Flash real size: %u bytes\n", realSize);
    Serial.printf( "      Flash ide  size: %u bytes\n", ideSize);
  }
  SPIFFS.begin();
  
  #ifdef DEBUG
    Serial.println("[5/9] Loading configuration...");
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
  
  #ifdef DEBUG
    Serial.println("[6/9] Starting Websocket Server...");
  #endif
  setupWebSocket();

  #ifdef DEBUG
    Serial.println("[7/9] Starting Webserver...");
  #endif
  setWebserver();
  
  #ifdef DEBUG
    Serial.println("[8/9] Starting RGB Controller...");
  #endif
  setupRGB();

  scanforwifi();
}

void loop(){

  // Scan for new Wifi Networks
  unsigned long currentScanMillis = millis();
  if (currentScanMillis - previousScanMillis >= scanInterval) {
    // save the last time you blinked the LED
    previousScanMillis = currentScanMillis;
    scanforwifi();
  }
  
}

void scanforwifi() {
  scanssids = "";
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
}
