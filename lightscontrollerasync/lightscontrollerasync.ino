#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
// SKETCH BEGIN
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

const char* ssid = "OPS_DSL";
const char* password = "ffeeddccbbaa";
const char * hostName = "SignSetup(async)";
const char* http_username = "admin";
const char* http_password = "admin";
const char compile_date[] = __DATE__ " " __TIME__;
bool forceon = true;

#include "webserver.h" // onboard webserver
#include "websocket.h" // websocket for communication/feedback to/from the frontend webpages



void setup(){
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.hostname(hostName);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(hostName);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(ssid, password);
  }

  MDNS.addService("http","tcp",80);

  SPIFFS.begin();

  ws.onEvent(onWsEvent);

  setWebserver();
}

void loop(){
  
}
