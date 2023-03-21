#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#include <WiFiUdp.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include "setting.h"

#include <string>
#include <stdlib.h>

void draw_countdown();
void draw_info();
void draw_note();
void draw_done();

/*const char *ssid = "你的WiFi_SSID";     // WiFi 名称
const char *password = "你的WiFi密码"; // WiFi 密码

const char *serverUrl_1 = "https://你的域名/sysinfo.php"; // 服务器地址，有 SSL 证书则使用 HTTPS 协议
const char *token_1 = "你的token";                        // 用于验证服务器的 token

const char *serverUrl_2 = "https://你的域名/sysinfo.php"; // 另一个服务器地址，有 SSL 证书则使用 HTTPS 协议
const char *token_2 = "你的token";                        // 用于验证服务器的 token*/

void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup()
{
  Serial.begin(115200); // 初始化串口通信，波特率为 115200
  pinMode(SetPin, OUTPUT);
  int SetPinState = digitalRead(SetPin);
  display.init();                 // 初始化显示器
  display.setI2cAutoInit(true);   // 开启I2C自动初始化
  display.flipScreenVertically(); // 翻转屏幕以正确显示
  display.clear();                // 清除屏幕上的内容

  // 读取FS json的配置
  Serial.println("mounting FS...");
  if (SPIFFS.begin())
  {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json"))
    {
      // file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile)
      {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);

#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get());
        serializeJson(json, Serial);
        if (!deserializeError)
        {
#else
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success())
        {
#endif
          Serial.println("\nparsed json");
          serverUrl_1 = json["serverUrl_1"].as<const char *>();
          serverUrl_2 = json["serverUrl_2"].as<const char *>();
          token = json["token"].as<const char *>();
        }
        else
        {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
  }
  else
  {
    Serial.println("failed to mount FS");
  }
  // end read

  WiFiManagerParameter custom_serverUrl_1("serverUrl_1", "serverUrl_1", serverUrl_1.c_str(), 40);
  WiFiManagerParameter custom_serverUrl_2("serverUrl_2", "serverUrl_2", serverUrl_2.c_str(), 40);
  WiFiManagerParameter custom_token("token", "token", token.c_str(), 32);
  WiFiManagerParameter custom_text("<p>点击SSID名称选择连接WiFi,并输入密码/服务器地址/设备token</p>");

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // add all your parameters here
  wifiManager.addParameter(&custom_serverUrl_1);
  wifiManager.addParameter(&custom_serverUrl_2);
  wifiManager.addParameter(&custom_token);
  wifiManager.addParameter(&custom_text);

  // 设置静态IP
  // wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // 在这里添加所有参数
  // wifiManager.addParameter(&custom_token);

  // 重置设置 - 用于测试
  // wifiManager.resetSettings();
  // SPIFFS.format();

  // 设置重置按钮的功能
  if (SetPinState == HIGH)
  {
    Serial.println("Getting Reset ESP Wifi-Setting.......");
    display.setFont(ArialMT_Plain_10);
    display.clear();
    display.drawXbm(35, 0, 60, 36, wifi_logo);
    display.drawString(0, 40, "RESET mode activated .");
    display.drawString(0, 50, "Please wait for reboot !");
    display.display();
    wifiManager.resetSettings();
    delay(5000);
    Serial.println("Formatting FS......");
    SPIFFS.format();
    delay(5000);
    Serial.println("Done Reboot In 5 seconds");
    draw_countdown();
/*    display.setFont(ArialMT_Plain_16);
    display.clear();
    display.drawString(5, 25, "Reboot in 5 Sec !");
    display.display();
    delay(1000);

    display.clear();
    display.drawString(5, 25, "Reboot in 4 Sec !");
    display.display();
    delay(1000);

    display.clear();
    display.drawString(5, 25, "Reboot in 3 Sec !");
    display.display();
    delay(1000);

    display.clear();
    display.drawString(5, 25, "Reboot in 2 Sec !");
    display.display();
    delay(1000);

    display.clear();
    display.drawString(5, 25, "Reboot in 1 Sec !");
    display.display();
    delay(1000);*/

    ESP.restart();
  }

  // 设置最低信号质量,如果信号质量低于8%,则略这些AP
  // 默认值为:8%
  // wifiManager.setMinimumSignalQuality();

  // 设置超时直到配置关闭
  // 有用的是让它全部重试或进入睡眠状态
  // wifiManager.setTimeout(120);

  // 获取ssid并传递并尝试连接
  // 如果它没有连接，它将启动具有指定名称的访问点,"AutoConnectAP",并进入等待配置的阻塞循环
  draw_note();
  if (!wifiManager.autoConnect("Get_Linux_Server_Info", ""))
  {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    // 重置并重试，或者让它深入睡眠
    ESP.reset();
    delay(1000);
  }

  // 如果已经连接
  Serial.println("WiFi connected)");
  draw_done();
  delay(3000);

  serverUrl_1 = custom_serverUrl_1.getValue();
  serverUrl_2 = custom_serverUrl_2.getValue();
  token = custom_token.getValue();
  Serial.println("The values in the file are: ");
  Serial.println("\tserverUrl_1: " + String(custom_serverUrl_1.getValue()));
  Serial.println("\tserverUrl_2: " + String(custom_serverUrl_2.getValue()));
  Serial.println("\ttoken: " + String(custom_token.getValue()));

  if (shouldSaveConfig)
  {
    Serial.println("saving config");
#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
    DynamicJsonDocument json(1024);
#else
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
#endif
    json["serverUrl_1"] = serverUrl_1.c_str();
    json["serverUrl_2"] = serverUrl_2.c_str();
    json["token"] = token.c_str();

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile)
    {
      Serial.println("failed to open config file for writing");
    }

#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
    serializeJson(json, Serial);
    serializeJson(json, configFile);
#else
    json.printTo(Serial);
    json.printTo(configFile);
#endif
    configFile.close();
    // end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  // Port defaults to 8266
  //ArduinoOTA.setPort(8266);

  ArduinoOTA.setHostname("Get_Linux_Server_Info");
  ArduinoOTA.onStart([]()
                     {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_SPIFFS
        type = "filesystem";
      }
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      } });
  ArduinoOTA.begin();

  display.init();                       // 初始化 OLED 显示屏
  display.clear();                      // 清空屏幕
  display.setFont(ArialMT_Plain_10);    // 设置字体
  display.flipScreenVertically();       // 垂直翻转屏幕
  while (WiFi.status() != WL_CONNECTED) // 等待连接成功
  {
    delay(1000);                        // 延迟 1 秒
    Serial.println("正在连接 WiFi..."); // 输出连接信息
  }
  Serial.println("WiFi 已经连接");           // 输出连接成功信息
  draw_info();
  /*display.clear();                           // 清空屏幕
  display.setTextAlignment(TEXT_ALIGN_LEFT); // 文字居左对齐
  display.drawXbm(35, 0, 60, 36, wifi_logo); // 在屏幕上绘制WiFi图标
  display.display();                         // 显示WiFi图标
  delay(3000);
  display.setFont(ArialMT_Plain_24);     // 设置字体大小
  display.clear();                       // 清空屏幕
  display.drawString(33, 5, "Server");   // 在屏幕上显示 "Server" 字符串
  display.drawString(28, 25, "Monitor"); // 在屏幕上显示 "Monitor" 字符串
  display.display();                     // 更新显示内容*/
}

void loop()
{
  ArduinoOTA.handle();
  HTTPClient http;             // 实例化HTTPClient对象
  WiFiClientSecure wifiClient; // 实例化WiFiClientSecure对象

  wifiClient.setInsecure(); // 允许不安全连接，非SSL可以取消

  // 组合访问链接
  String url_1 = (String(serverUrl_1.c_str()) + "/sysinfo.php?token=" + token.c_str());
  String url_2 = (String(serverUrl_2.c_str()) + "/sysinfo.php?token=" + token.c_str());

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
      display.setFont(ArialMT_Plain_16);         // 设置字体
      display.clear();
      display.drawString(0, 23, serverUrl_1.c_str()); // 显示网站名称
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
      Serial.print(String(serverUrl_2.c_str()) + "/sysinfo.php?token=" + token.c_str());
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
      display.setFont(ArialMT_Plain_16);
      display.clear();
      display.drawString(0, 23, serverUrl_2.c_str());
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
      Serial.print(String(serverUrl_2.c_str()) + "/sysinfo.php?token=" + token.c_str());
    }

    http.end();
  }
  else
  {
    Serial.println("Failed to connect to server");
  }
  delay(5000); // 不能太快
}
