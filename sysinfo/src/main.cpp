#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "SSD1306Wire.h"

SSD1306Wire display(0x3c, D2, D1);
const char *ssid = "chrisxs_01";
const char *password = "aj20160223";
const char *serverUrl = "https://chrisxs.xyz/sysinfo.php"; // 无SSL证书则："http://IP或者域名/sysinfo.php"
const char *token = "ZsA*M772Uxzv";                        //

void setup()
{
  Serial.begin(115200);
  display.init();
  display.setFont(ArialMT_Plain_10);
  display.flipScreenVertically();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("正在连接WiFi...");
  }
  Serial.println("WiFi已经连接");
}

void loop()
{
  HTTPClient http;
  WiFiClientSecure wifiClient;

  wifiClient.setInsecure(); // 允许不安全连接，非SSL可以取消

  String url = String(serverUrl) + "?token=" + String(token);

  if (http.begin(wifiClient, url))
  {
    int httpCode = http.GET();

    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println(payload);
      delay(5000);
      // 使用字符串的截取和分割函数
      int cpuIndex = payload.indexOf("CPU Usage:");
      int memIndex = payload.indexOf("Memory Usage:");
      int swapIndex = payload.indexOf("Swap Usage:");
      int rIndex = payload.indexOf("Rx:");
      int tIndex = payload.indexOf("Tx:");
      String cpuUsage = payload.substring(cpuIndex + 11, memIndex - 1);
      String memUsage = payload.substring(memIndex + 14, swapIndex - 1);
      String swapUsage = payload.substring(swapIndex + 12, rIndex - 1);
      String rRate = payload.substring(rIndex + 2, tIndex - 1);
      String tRate = payload.substring(tIndex + 2);
      // Serial.println("CPU Usage: " + cpuUsage);
      // Serial.println("Memory Usage: " + memUsage);
      // Serial.println("Swap Usage: " + swapUsage);
      // Serial.println("RX Rate: " + rRate);
      // Serial.println("TX Rate: " + tRate);

      // 显示
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_24);
      display.clear();
      display.drawString(0, 5, "Server Info");
      display.setFont(ArialMT_Plain_16);
      display.drawString(0, 30, "CPU : " + cpuUsage);
      display.drawString(0, 45, "Memory : " + memUsage);
      display.display();
      delay(5000);
      display.clear();
      display.drawString(0, 5, "Swap: " + swapUsage);
      display.drawString(0, 25, "Rx  " + rRate);
      display.drawString(0, 45, "Tx  " + tRate);
      display.display();
    }
    else
    {
      Serial.printf("HTTP error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  else
  {
    Serial.println("Failed to connect to server");
  }
  delay(5000);
}
