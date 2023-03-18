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
#include <WiFiClientSecure.h>

const char *ssid = "你的WiFi_SSID";
const char *password = "你的WiFi_密码";

// 服务器URL
const char *serverName = "https://chrisxs.com/espdb/esp-post-data.php";
// https服务器URL
// const char *serverName = "https://chrisxs.com/espdb/esp-post-data.php";

// apiKeyValue为随机生成，必须与esp-database.php内的API KEY保持一致
String apiKeyValue = "你的数据库API_KEY";
String sensorName = "BME280";
String sensorLocation = "office";

/*#include <SPI.h>
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
// Adafruit_BME280 bme(BME_CS);  // 硬件 SPI
// Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);  // 软件 SPI

// 下面的变量是无符号长整型，因为时间（以毫秒为单位）会很快成为比 int 类型能够存储的更大的数字。
unsigned long lastTime = 0;
// 定时器设置为10分钟（600000）
// unsigned long timerDelay = 600000;
// 将定时器设置为30秒（30000）
unsigned long timerDelay = 5000;

void setup()
{
  Serial.begin(115200); // 初始化串口通信，波特率为115200

  WiFi.begin(ssid, password);           // 连接WiFi网络，需要提供SSID和密码
  Serial.println("Connecting");         // 在串口打印提示信息：正在连接WiFi
  while (WiFi.status() != WL_CONNECTED) // 循环等待WiFi连接成功
  {
    delay(500);        // 暂停500毫秒
    Serial.print("."); // 在串口打印提示信息：正在连接，每次循环打印一个点
  }
  Serial.println("");                                          // 在串口打印空行，作为分隔符
  Serial.print("Connected to WiFi network with IP Address: "); // 在串口打印提示信息：已连接到WiFi网络，显示IP地址
  Serial.println(WiFi.localIP()); 
  Serial.println(WiFi.SSID()); 
  Serial.println(WiFi.RSSI());                             // 在串口打印本地IP地址

  // 初始化BME280传感器，使用I2C接口，I2C地址为0x76
  bool status = bme.begin(); // bme.begin(0x76)
  if (!status)               // 如果初始化失败
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring or change I2C address!"); // 在串口打印错误提示信息：无法找到有效的BME280传感器，检查连接或更改I2C地址！
    while (1)                                                                                    // 进入死循环，程序不再继续执行
      ;
  }

  Serial.println("Timer set to 30 seconds (timerDelay variable), it will take 30 seconds before publishing the first reading."); // 在串口打印提示信息：定时器设置为30秒（timerDelay变量），在发布第一次读数之前需要30秒。
}

void loop()
{
  // 每隔 10 分钟发送一次 HTTP POST 请求
  if ((millis() - lastTime) > timerDelay)
  {
    // 检查 WiFi 连接状态
    if (WiFi.status() == WL_CONNECTED)
    {
      WiFiClientSecure client; // HTTP则注释掉
      client.setInsecure();    // HTTP则注释掉
      HTTPClient http;

      // 填写你的域名或 IP 地址以及 URL 路径
      http.begin(client, serverName);
      // HTTP则：
      // http.begin(serverName);

      // 指定 content-type 头部
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // 准备 HTTP POST 请求数据
      String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName + "&location=" + sensorLocation + "&value1=" + String(bme.readTemperature()) + "&value2=" + String(bme.readHumidity()) + "&value3=" + String(bme.readPressure() / 100.0F) + "";
      Serial.print("httpRequestData: ");
      Serial.println(httpRequestData);

      // 你可以注释上面的 httpRequestData 变量
      // 然后使用下面的 httpRequestData 变量（用于没有 BME280 传感器的测试目的）
      // String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";

      // 发送 HTTP POST 请求
      int httpResponseCode = http.POST(httpRequestData);

      // 如果需要使用 content-type：text/plain 的 HTTP 请求
      // http.addHeader("Content-Type", "text/plain");
      // int httpResponseCode = http.POST("Hello, World!");

      // 如果需要使用 content-type：application/json 的 HTTP 请求，使用以下代码：
      // http.addHeader("Content-Type", "application/json");
      // int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");

      if (httpResponseCode > 0)
      {
        Serial.print("HTTP 响应代码：");
        Serial.println(httpResponseCode);
      }
      else
      {
        Serial.print("错误代码：");
        Serial.println(httpResponseCode);
      }
      // 释放资源
      http.end();
    }
    else
    {
      Serial.println("WiFi 未连接");
    }
    lastTime = millis();
  }
}