// 这里会因应你所选用的板子而自动切换需要的板库
#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#endif

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFiClientSecure.h> //HTTPS

// 替换掉下面的WiFi,填入你的
const char *ssid = "你的WiFi_SSID";
const char *password = "你的WiFi密码";

// 填入你的主机地址（HTTPS）
const char *serverName = "https://你的域名/esp-chart-post-data.php";

// 填入你的主机地址（HTTP）
//const char *serverName = "http://你的域名/esp-chart-post-data.php";

// 这里的api key 必须要与 esp-chart-post-data.php 文件内的APIKEY相同
String apiKeyValue = "Laq8mL6GHjdZLdHA";

Adafruit_BME280 bme; // I2C

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // 万一无法连接BEM 280串口会提示反馈错误信息,可以直接用bme.begin(0x76)跳过
  bool status = bme.begin(0x76);
  if (!status)
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring or change I2C address!");
    while (1)
      ;
  }
}

void loop()
{
  // 检查WiFi连接状况
  if (WiFi.status() == WL_CONNECTED)
  {
    // HTTPS则如下开启
    WiFiClientSecure client;
    client.setInsecure();
    //如果是HTTP，则注释以上两句
    HTTPClient http;

    // https
    http.begin(client,serverName);

    // http
    //http.begin(serverName);

    // 指定内容类型标题
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // 准备HTTP POST请求数据
    String httpRequestData = "api_key=" + apiKeyValue + "&value1=" + String(bme.readTemperature()) + "&value2=" + String(bme.readHumidity()) + "&value3=" + String(bme.readPressure() / 100.0F) + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    // 在上面注释httpRequestData变量然后，使用下面的httpRequestData变量
    // （出于测试目的，绕过BME280传感器）
    // String httpRequestData = "api_key=tPmAT5Ab3j7F9&value1=24.75&value2=49.54&value3=1005.14";
    // 发送POST请求
    int httpResponseCode = http.POST(httpRequestData);

    // 如果需要具有以下内容类型的HTTP请求：文本/纯文本
    // http.addHeader("Content-Type", "text/plain");
    // int httpResponseCode = http.POST("Hello, World!");

    // 如果需要以下内容类型的HTTP请求：application / json，请使用以下命令：
    // http.addHeader("Content-Type", "application/json");
    // int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");

    if (httpResponseCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }
  // 每隔30秒发送一次post请求
  delay(30000);
}
