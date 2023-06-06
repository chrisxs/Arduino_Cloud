#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>        // 引入I2C库
#include "SSD1306Wire.h" // 引入OLED驱动库

// 初始化OLED对象
SSD1306Wire display(0x3c, 2, 14); 


const char *ssid = "WiFi_SSID";
const char *password = "WiFi密码";

const char *server = "你的服务器地址或者路径";
const int port = 80;
const char *apiPath = "/zabbix/api_jsonrpc.php";
const char *apiToken = "你的apiToken";
const char *hostname = "主机名";

void printCPUUsage();
void printMemoryUsage();
void printSWAPUsage();
void printBootTime();

void draw_info();      // 定义函数，用于绘制信息界面

// 定义 API 请求字符串变量
String apiRequest_bootTime = String() +
                             "{"
                             "\"jsonrpc\": \"2.0\","
                             "\"method\": \"item.get\","
                             "\"params\": {"
                             "\"output\": [\"lastvalue\"],"
                             "\"search\": {"
                             "\"key_\": \"system.boottime\""
                             "},"
                             "\"host\": \"" + String(hostname) + "\""
                             "},"
                             "\"auth\": \"" + String(apiToken) + "\","
                             "\"id\": 1"
                             "}";

String apiRequest_swap = String() +
                          "{"
                          "\"jsonrpc\": \"2.0\","
                          "\"method\": \"item.get\","
                          "\"params\": {"
                          "\"output\": [\"lastvalue\"],"
                          "\"search\": {"
                          "\"key_\": \"system.swap.size[,pfree]\""
                          "},"
                          "\"host\": \"" + String(hostname) + "\""
                          "},"
                          "\"auth\": \"" + String(apiToken) + "\","
                          "\"id\": 1"
                          "}";



String apiRequest_cpu = String() +
                        "{"
                        "\"jsonrpc\": \"2.0\","
                        "\"method\": \"item.get\","
                        "\"params\": {"
                        "\"output\": [\"lastvalue\"],"
                        "\"host\": \"" +
                        String(hostname) + "\","
                        "\"search\": {"
                        "\"key_\": \"system.cpu.load[all,avg1]\""
                        "},"
                        "\"sortfield\": \"name\""
                        "},"
                        "\"auth\": \"" +
                        String(apiToken) + "\","
                        "\"id\": 1"
                        "}";

String apiRequest_men = String() +
                        "{"
                        "\"jsonrpc\": \"2.0\","
                        "\"method\": \"item.get\","
                        "\"params\": {"
                        "\"output\": [\"lastvalue\"],"
                        "\"host\": \"" +
                        String(hostname) + "\","
                        "\"search\": {"
                        "\"key_\": \"vm.memory.size[available]\""
                        "},"
                        "\"sortfield\": \"name\""
                        "},"
                        "\"auth\": \"" +
                        String(apiToken) + "\","
                        "\"id\": 1"
                        "}";

void setup()
{
  Serial.begin(115200);

  display.init();                 // 初始化显示器
  display.setI2cAutoInit(true);   // 开启I2C自动初始化
  display.flipScreenVertically(); // 翻转屏幕以正确显示
  display.clear();                // 清除屏幕上的内容

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("正在连接到 Wi-Fi...");
  }
  Serial.println("已连接到 Wi-Fi!");

  printCPUUsage();
  printMemoryUsage();
  printSWAPUsage();
  printBootTime();
}

void loop()
{
printCPUUsage();
printMemoryUsage();
printSWAPUsage();
printBootTime();
delay(5000);
}

void printCPUUsage()
{
  WiFiClient client;
  if (client.connect(server, port))
  {
    String request = String() +
                     "POST " + apiPath + " HTTP/1.1\r\n" +
                     "Host: " + server + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + String(apiRequest_cpu.length()) + "\r\n\r\n" +
                     apiRequest_cpu;

    client.print(request);

    unsigned long timeout = millis();
    while (client.available() == 0)
    {
      if (millis() - timeout > 5000)
      {
        Serial.println("错误：服务器没有响应！");
        client.stop();
        return;
      }
    }

    // 跳过响应头
    while (client.available())
    {
      String line = client.readStringUntil('\r');
      // Serial.println(line);
      if (line == "\n")
      {
        break; // 找到分隔响应头和响应体的空行
      }
    }

    // 读取并解析 JSON 数据
    String json = "";
    while (client.available())
    {
      json += client.readString();
    }

    // 从 JSON 中提取 CPU 占用率
    int startIndex = json.indexOf("\"lastvalue\":\"") + 13;
    int endIndex = json.indexOf("\"", startIndex);
    String cpuUsage = json.substring(startIndex, endIndex);
    float cpuUsageFloat = cpuUsage.toFloat();
    float cpuUsagePercentage = cpuUsageFloat * 100;
    int roundedPercentage = round(cpuUsagePercentage); // 将百分比四舍五入为最接近的整数
    String formattedPercentage = String(cpuUsagePercentage, 2); // 将百分比格式化为保留两位小数的字符串

    // Serial.println(json);

    // 打印 CPU 占用率
    display.clear();                           // 清空屏幕
    display.setTextAlignment(TEXT_ALIGN_LEFT); // 文字居左对齐
    display.setFont(ArialMT_Plain_24);     // 设置字体大小
    display.drawString(0, 5,"CPU Usage" );
    display.drawString(25, 35,String(formattedPercentage)+" %");
    display.display();                         
    Serial.print("CPU占用率：");
    Serial.println(cpuUsage);
    client.stop();
  }
  else
  {
    Serial.println("错误：无法连接到 Zabbix 服务器！");
  }
}

void printMemoryUsage()
{
  WiFiClient client;
  if (client.connect(server, port))
  {
    String request = String() +
                     "POST " + apiPath + " HTTP/1.1\r\n" +
                     "Host: " + server + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + String(apiRequest_men.length()) + "\r\n\r\n" +
                     apiRequest_men;

    client.print(request);

    unsigned long timeout = millis();
    while (client.available() == 0)
    {
      if (millis() - timeout > 5000)
      {
        Serial.println("错误：服务器没有响应！");
        client.stop();
        return;
      }
    }

    // 跳过响应头
    while (client.available())
    {
      String line = client.readStringUntil('\r');
      // Serial.println(line);
      if (line == "\n")
      {
        break; // 找到分隔响应头和响应体的空行
      }
    }

    // 读取并解析 JSON 数据
    String json = "";
    while (client.available())
    {
      json += client.readString();
    }
    // 从 JSON 中提取可用内存值
    int startIndex = json.indexOf("\"lastvalue\":\"") + 13;
    int endIndex = json.indexOf("\"", startIndex);
    String memoryValue = json.substring(startIndex, endIndex);
    // Serial.println(json);

    // 将字节数转换为 GB
    double memoryGB = memoryValue.toDouble() / 1024 / 1024 / 1024;

    // 打印可用内存值
    display.clear();                           // 清空屏幕
    display.setTextAlignment(TEXT_ALIGN_LEFT); // 文字居左对齐
    display.setFont(ArialMT_Plain_24);     // 设置字体大小
    display.drawString(0, 5,"FreeMem" );
    display.drawString(20, 35,String(memoryGB)+" GB");
    display.display();      
    Serial.print("可用内存：");
    Serial.print(memoryGB);
    Serial.println(" GB");

    client.stop();
  }
  else
  {
    Serial.println("错误：无法连接到 Zabbix 服务器！");
  }
}



void printSWAPUsage() {
  WiFiClient client;
  if (client.connect(server, port)) { // 连接到服务器
    String request = String() +
                     "POST " + apiPath + " HTTP/1.1\r\n" +
                     "Host: " + server + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + String(apiRequest_swap.length()) + "\r\n\r\n" +
                     apiRequest_swap;

    client.print(request); // 发送请求

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) { // 超时处理
        Serial.println("错误：服务器未响应！");
        client.stop();
        return;
      }
    }

    // 跳过响应头部
    while (client.available()) {
      String line = client.readStringUntil('\r');
      if (line == "\n") {
        break; // 找到分隔响应头部和正文的空行
      }
    }

    // 读取和解析 JSON 数据
    String json = "";
    while (client.available()) {
      json += client.readString();
    }

    // 从 JSON 中提取 SWAP 使用率的值
    int startIndex = json.indexOf("\"lastvalue\":\"") + 13;
    int endIndex = json.indexOf("\"", startIndex);
    String swapUsagePercentage = json.substring(startIndex, endIndex);
    //Serial.println(json);

    // 将 SWAP 使用率从百分比转换为 GB
    float swapUsageRatio = swapUsagePercentage.toFloat() / 100;
    float swapTotalSize = 1024; // 用实际的 SWAP 总大小（以 MB 为单位）替换
    float swapUsageInGB = (swapUsageRatio * swapTotalSize) / 1024;

    // 打印 SWAP 使用情况
    display.clear();                           // 清空屏幕
    display.setTextAlignment(TEXT_ALIGN_LEFT); // 文字居左对齐
    display.setFont(ArialMT_Plain_24);     // 设置字体大小
    display.drawString(0, 5,"Free SWAP" );
    display.drawString(20, 35,String(swapUsageInGB)+" GB");
    display.display();    
    Serial.print("SWAP剩余：");
    Serial.print(swapUsageInGB, 2); // 保留两位小数打印
    Serial.println(" GB");
    client.stop();
  }
  else {
    Serial.println("错误：无法连接到 Zabbix 服务器！");
  }
}


void printBootTime() {
  WiFiClient client;
  if (client.connect(server, port)) {
    String request = String() +
                     "POST " + apiPath + " HTTP/1.1\r\n" +
                     "Host: " + server + "\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + String(apiRequest_bootTime.length()) + "\r\n\r\n" +
                     apiRequest_bootTime;

    client.print(request);

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("Error: 服务器无响应！");
        client.stop();
        return;
      }
    }

    // 跳过响应头部
    while (client.available()) {
      String line = client.readStringUntil('\r');
      if (line == "\n") {
        break; // 找到分隔响应头部和主体的空行
      }
    }

    // 读取和解析 JSON 数据
    String json = "";
    while (client.available()) {
      json += client.readString();
    }

    // 从 JSON 中提取启动时间的值
    int startIndex = json.indexOf("\"lastvalue\":\"") + 13;
    int endIndex = json.indexOf("\"", startIndex);
    String bootTime = json.substring(startIndex, endIndex);

    // 将启动时间从 UNIX 时间戳转换为可读格式
    unsigned long bootTimeUnix = bootTime.toInt();
    time_t bootTimeSeconds = bootTimeUnix;
    struct tm *timeinfo;
    setenv("TZ", "CST-8", 1); // 设置时区为中国标准时间 (CST)
    tzset();
    timeinfo = localtime(&bootTimeSeconds);
    char bootTimeStr[20];
    strftime(bootTimeStr, sizeof(bootTimeStr), "%Y-%m-%d %H:%M:%S", timeinfo);

    // 打印启动时间
    display.clear();                           // 清空屏幕
    display.setTextAlignment(TEXT_ALIGN_LEFT); // 文字居左对齐
    display.setFont(ArialMT_Plain_24);     // 设置字体大小
    display.drawString(0, 5,"Boot From" );
    display.setFont(ArialMT_Plain_10);     // 设置字体大小
    display.drawString(0, 35,String(bootTimeStr));
    display.display();    
    Serial.print("启动时间: ");
    Serial.println(bootTimeStr);

    client.stop();
  }
  else {
    Serial.println("Error: 无法连接到 Zabbix 服务器！");
  }
}

