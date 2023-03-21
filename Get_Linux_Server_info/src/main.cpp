#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "SSD1306Wire.h"

SSD1306Wire display(0x3c, 2, 14); // OLED 显示屏初始化，0x3c 是 OLED 地址，2 是 SDA 引脚，14 是 SCL 引脚

const char *ssid = "你的WiFi_SSID";     // WiFi 名称
const char *password = "你的WiFi密码"; // WiFi 密码

const char *serverUrl_1 = "https://你的域名/sysinfo.php"; // 服务器地址，有 SSL 证书则使用 HTTPS 协议
const char *token_1 = "你的token";                        // 用于验证服务器的 token

const char *serverUrl_2 = "https://你的域名/sysinfo.php"; // 另一个服务器地址，有 SSL 证书则使用 HTTPS 协议
const char *token_2 = "你的token";                        // 用于验证服务器的 token

void setup()
{
  Serial.begin(115200);                 // 初始化串口通信，波特率为 115200
  display.init();                       // 初始化 OLED 显示屏
  display.clear();                      // 清空屏幕
  display.setFont(ArialMT_Plain_10);    // 设置字体
  display.flipScreenVertically();       // 垂直翻转屏幕
  WiFi.begin(ssid, password);           // 连接 WiFi 网络
  while (WiFi.status() != WL_CONNECTED) // 等待连接成功
  {
    delay(1000);                        // 延迟 1 秒
    Serial.println("正在连接 WiFi..."); // 输出连接信息
  }
  Serial.println("WiFi 已经连接");           // 输出连接成功信息
  display.clear();                           // 清空屏幕
  display.setTextAlignment(TEXT_ALIGN_LEFT); // 文字居左对齐
  display.setFont(ArialMT_Plain_24);         // 设置字体大小
  display.clear();                           // 清空屏幕
  display.drawString(33, 5, "Server");       // 在屏幕上显示 "Server" 字符串
  display.drawString(28, 25, "Monitor");     // 在屏幕上显示 "Monitor" 字符串
  display.display();                         // 更新显示内容
}

void loop()
{
  HTTPClient http;             // 实例化HTTPClient对象
  WiFiClientSecure wifiClient; // 实例化WiFiClientSecure对象

  wifiClient.setInsecure(); // 允许不安全连接，非SSL可以取消

  // 组合访问链接
  String url_1 = String(serverUrl_1) + "?token=" + String(token_1);
  String url_2 = String(serverUrl_2) + "?token=" + String(token_2);

  // 连接第一个服务器
  if (http.begin(wifiClient, url_1))
  {
    int httpCode = http.GET(); // 发送GET请求

    if (httpCode > 0)
    {
      String payload = http.getString(); // 获取响应内容
      Serial.println(payload);           // 输出响应内容
      delay(1000);

      // 使用字符串的截取和分割函数
      int cpuIndex = payload.indexOf("CPU Usage:");
      int memIndex = payload.indexOf("Memory Usage:");
      int swapIndex = payload.indexOf("Swap Usage:");
      int rIndex = payload.indexOf("Rx:");
      int tIndex = payload.indexOf("Tx:");

      String cpuUsage = payload.substring(cpuIndex + 11, memIndex - 1);  // 获取CPU使用率
      String memUsage = payload.substring(memIndex + 14, swapIndex - 1); // 获取内存使用率
      String swapUsage = payload.substring(swapIndex + 12, rIndex - 1);  // 获取Swap使用率
      String rRate = payload.substring(rIndex + 2, tIndex - 1);          // 获取网络接收速率
      String tRate = payload.substring(tIndex + 2);                      // 获取网络发送速率

      // 显示
      display.clear();                           // 清除屏幕内容
      display.setTextAlignment(TEXT_ALIGN_LEFT); // 设置文本左对齐
      display.setFont(ArialMT_Plain_24);         // 设置字体
      display.clear();
      display.drawString(0, 23, "你的域名"); // 显示网站名称
      display.display();
      display.clear();
      delay(3000); // 等待3秒

      display.setFont(ArialMT_Plain_16);                 // 设置字体大小
      display.drawString(0, 0, "CPU : " + cpuUsage);     // 显示CPU使用率
      display.drawString(0, 20, "Memory : " + memUsage); // 显示内存使用率
      display.drawString(0, 40, "Swap: " + swapUsage);   // 显示Swap使用率
      display.display();
      delay(3000); // 等待3秒

      // display.drawString(0, 25, "Rx  " + rRate);
      // display.drawString(0, 45, "Tx  " + tRate);
      // display.display();
    }
    else
    {
      Serial.printf("HTTP error: %s\n", http.errorToString(httpCode).c_str()); // 输出HTTP错误信息
    }

    http.end(); // 结束HTTP连接
  }
  else
  {
    Serial.println("连接服务器失败"); // 连接服务器失败
  }
  delay(5000); // 间隔5秒发送下一次请求

  if (http.begin(wifiClient, url_2))
  {
    int httpCode = http.GET();

    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println(payload);
      delay(1000);
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
      display.drawString(0, 23, "你的域名");
      display.display();
      display.clear();
      delay(3000);
      display.setFont(ArialMT_Plain_16);
      display.drawString(0, 0, "CPU : " + cpuUsage);
      display.drawString(0, 20, "Memory : " + memUsage);
      display.drawString(0, 40, "Swap: " + swapUsage);
      display.display();
      delay(3000);
      // display.drawString(0, 25, "Rx  " + rRate);
      // display.drawString(0, 45, "Tx  " + tRate);
      // display.display();
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
  delay(5000); // 不能太快
}
