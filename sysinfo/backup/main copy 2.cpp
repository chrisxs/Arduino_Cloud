#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "WiFi_SSID";
const char* password = "WiFi_SSID";
const char* serverUrl = "https://域名或IP/sysinfo.php";
const char* token = "密钥";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  HTTPClient http;
  WiFiClientSecure wifiClient;

  wifiClient.setInsecure(); // 允许不安全连接

  String url = String(serverUrl) + "?token=" + String(token);

  if (http.begin(wifiClient, url)) {
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.printf("HTTP error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("Failed to connect to server");
  }

  delay(5000);
}
