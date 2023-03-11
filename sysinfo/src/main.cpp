#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "SSD1306Wire.h" 

SSD1306Wire display(0x3c, D5, D6);
const char *ssid = "你的WiFi SSID";
const char *password = "你的WiFi密码";
const char *serverUrl = "https://IP或者域名/sysinfo.php"; //无SSL证书则："http://IP或者域名/sysinfo.php"
const char *token = "你的自定义密钥"; //

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

  wifiClient.setInsecure(); //允许不安全连接，非SSL可以取消

  String url = String(serverUrl) + "?token=" + String(token);

  if (http.begin(wifiClient, url))
  {
    int httpCode = http.GET();

    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println(payload);
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_10);
      display.drawString(0, 5, String(payload));
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
  delay(3000);
}
