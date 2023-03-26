#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "wifi_setting.h"
#include "OTA_setting.h"

#include <FS.h>



void setup()
{
  Serial.begin(115200);
  SPIFFS.begin();
  WiFi.mode(WIFI_STA);
  delay(1000);
  if (!loadConfig())
  {
    WiFi.softAP(ssid, password);
    IPAddress apIP(192, 168, 4, 1);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    delay(1000);
    OTA();
    server.on("/", handleRoot);
    server.on("/save", handleSave);
    server.on("/restart", HTTP_POST, handleRestart);
    server.on("/clear", HTTP_POST, handleClear);
    server.begin();
    Serial.println("设备处于 AP 模式");
  }
  else
  {
    connectWiFi();
  }
  OTA();
  server.begin();
  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.on("/restart", HTTP_POST, handleRestart);
  server.on("/clear", HTTP_POST, handleClear);
}

void loop()
{
  server.handleClient();
  OTAserver.handleClient();
}
