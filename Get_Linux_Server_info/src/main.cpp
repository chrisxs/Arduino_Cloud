#include <Arduino.h>
#include <ESP8266WiFi.h>       // 引用ESP8266WiFi库，用于连接WiFi
#include <ESP8266HTTPClient.h> // 引用ESP8266HTTPClient库，用于向服务器发送HTTP请求

#include <ESP8266mDNS.h>      // 引用ESP8266mDNS库，用于将设备注册到局域网内的mDNS服务器
#include <ESP8266WebServer.h> // 引用ESP8266WebServer库，用于创建Web服务器

#include <WiFiUdp.h>     // 引用WiFiUdp库，用于发送UDP数据包
#include <WiFiManager.h> // 引用WiFiManager库，用于简化WiFi连接和管理
#include <DNSServer.h>   // 引用DNSServer库，用于将特定的DNS请求重定向到设备
#include <ArduinoOTA.h>  // 引用ArduinoOTA库，用于通过WiFi进行OTA固件更新
#include <ArduinoJson.h> // 引用ArduinoJson库，用于解析JSON数据
#include "setting.h"     // 引用自定义的setting.h头文件，用于保存设备配置

#include <string>   // 引用string库，用于处理字符串
#include <stdlib.h> // 引用stdlib.h库，用于实现C++标准库函数

void saveConfigCallback() // 定义函数，用于保存设备配置
{
  Serial.println("Should save config"); // 打印提示信息，表明需要保存配置
  shouldSaveConfig = true;              // 设置标志位shouldSaveConfig为true，以便在循环中保存配置
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
  Serial.println("mounting FS..."); // 打印串口信息：挂载文件系统
  if (SPIFFS.begin())               // 如果SPIFFS文件系统成功挂载
  {
    Serial.println("mounted file system"); // 打印串口信息：文件系统已挂载
    if (SPIFFS.exists("/config.json"))     // 如果config.json文件存在于SPIFFS中
    {
      // 文件存在，正在读取并加载配置信息
      Serial.println("reading config file");              // 打印串口信息：正在读取配置文件
      File configFile = SPIFFS.open("/config.json", "r"); // 打开配置文件
      if (configFile)                                     // 如果配置文件打开成功
      {
        Serial.println("opened config file"); // 打印串口信息：配置文件已打开
        size_t size = configFile.size();      // 获取配置文件大小
        // 分配一个缓冲区来存储文件内容。
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size); // 读取配置文件内容

#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
        DynamicJsonDocument json(1024);                           // 创建一个json文档
        auto deserializeError = deserializeJson(json, buf.get()); // 反序列化JSON
        serializeJson(json, Serial);                              // 将JSON序列化并打印到串口
        if (!deserializeError)                                    // 如果反序列化成功
        {
#else
        DynamicJsonBuffer jsonBuffer;                         // 创建一个json缓冲区
        JsonObject &json = jsonBuffer.parseObject(buf.get()); // 将配置文件解析成json对象
        json.printTo(Serial);                                 // 打印JSON到串口
        if (json.success())                                   // 如果解析成功
        {
#endif
          Serial.println("\nparsed json");                      // 打印串口信息：JSON解析成功
          serverUrl_1 = json["serverUrl_1"].as<const char *>(); // 从JSON中读取serverUrl_1
          serverUrl_2 = json["serverUrl_2"].as<const char *>(); // 从JSON中读取serverUrl_2
          token = json["token"].as<const char *>();             // 从JSON中读取token
        }
        else // 如果解析失败
        {
          Serial.println("failed to load json config"); // 打印串口信息：JSON配置加载失败
        }
        configFile.close(); // 关闭配置文件
      }
    }
  }
  else // 如果SPIFFS文件系统挂载失败
  {
    Serial.println("failed to mount FS"); // 打印串口信息：文件系统挂载失败
  }
  // 结束读取配置文件

  WiFiManagerParameter custom_serverUrl_1("serverUrl_1", "serverUrl_1", serverUrl_1.c_str(), 40);      // 自定义参数1，用于设置服务器地址1，最大长度为40
  WiFiManagerParameter custom_serverUrl_2("serverUrl_2", "serverUrl_2", serverUrl_2.c_str(), 40);      // 自定义参数2，用于设置服务器地址2，最大长度为40
  WiFiManagerParameter custom_token("token", "token", token.c_str(), 32);                              // 自定义参数3，用于设置设备 token，最大长度为32
  WiFiManagerParameter custom_text("<p>点击SSID名称选择连接WiFi,并输入密码/服务器地址/设备token</p>"); // 自定义参数4，显示在 WiFiManager 界面上的文本提示

  // WiFiManager 实例化
  WiFiManager wifiManager;

  // 设置配置保存回调函数
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // 添加自定义参数
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
  if (SetPinState == HIGH) // 如果重置按钮被按下
  {
    Serial.println("Getting Reset ESP Wifi-Setting.......");
    display.setFont(ArialMT_Plain_10);
    display.clear();
    display.drawXbm(35, 0, 60, 36, wifi_logo);
    display.drawString(0, 40, "RESET mode activated .");
    display.drawString(0, 50, "Please wait for reboot !");
    display.display();
    wifiManager.resetSettings();//重置ESP的Wifi设置
        delay(5000);
    Serial.println("Formatting FS......");
    SPIFFS.format(); // 格式化SPIFFS文件系统
    delay(5000);
    Serial.println("Done Reboot In 5 seconds");
    draw_countdown(); // 显示倒计时
    ESP.restart();    // 重启ESP
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
Serial.println("WiFi connected)"); // 打印WiFi已连接
draw_done(); // 调用函数画图完成
delay(3000); // 延迟3秒

// 获取自定义服务器地址和token
serverUrl_1 = custom_serverUrl_1.getValue(); // 获取自定义服务器地址1
serverUrl_2 = custom_serverUrl_2.getValue(); // 获取自定义服务器地址2
token = custom_token.getValue(); // 获取自定义token
Serial.println("The values in the file are: "); // 打印文件中的值
Serial.println("\tserverUrl_1: " + String(custom_serverUrl_1.getValue())); // 打印自定义服务器地址1的值
Serial.println("\tserverUrl_2: " + String(custom_serverUrl_2.getValue())); // 打印自定义服务器地址2的值
Serial.println("\ttoken: " + String(custom_token.getValue())); // 打印自定义token的值

// 如果需要保存配置
if (shouldSaveConfig)
{
  Serial.println("saving config"); // 打印正在保存配置
#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
  DynamicJsonDocument json(1024); // 创建动态JSON文档对象
#else
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject(); // 创建JSON对象
#endif
  json["serverUrl_1"] = serverUrl_1.c_str(); // 添加自定义服务器地址1到JSON对象
  json["serverUrl_2"] = serverUrl_2.c_str(); // 添加自定义服务器地址2到JSON对象
  json["token"] = token.c_str(); // 添加自定义token到JSON对象

  File configFile = SPIFFS.open("/config.json", "w"); // 打开配置文件以写入
  if (!configFile)
  {
    Serial.println("failed to open config file for writing"); // 打开配置文件失败
  }

#ifdef ARDUINOJSON_VERSION_MAJOR >= 6
  serializeJson(json, Serial); // 将JSON对象序列化并打印到串口
  serializeJson(json, configFile); // 将JSON对象序列化并写入配置文件
#else
  json.printTo(Serial); // 将JSON对象打印到串口
  json.printTo(configFile); // 将JSON对象写入配置文件
#endif
  configFile.close(); // 关闭配置文件
  // end save
}

Serial.println("local ip： "+ String(WiFi.localIP().toString())); // 打印本地IP地址

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

ArduinoOTA.setHostname("Get_Linux_Server_Info"); // 设置 OTA 主机名
ArduinoOTA.onStart([]() // OTA 开始时执行的函数
{
  String type;
  if (ArduinoOTA.getCommand() == U_FLASH) { // 判断是固件更新还是文件系统更新
    type = "sketch";
  } else {
    type = "filesystem";
  }
  // 注意：如果更新文件系统，此处应卸载文件系统
  Serial.println("开始更新 " + type); // 打印更新信息
});

ArduinoOTA.onEnd([]() // OTA 结束时执行的函数
{
  Serial.println("\n更新结束"); // 打印更新结束信息
});

ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) // OTA 更新进度发生改变时执行的函数
{
  Serial.printf("更新进度: %u%%\r", (progress / (total / 100))); // 打印更新进度信息
});

ArduinoOTA.onError([](ota_error_t error) // OTA 更新出错时执行的函数
{
  Serial.printf("错误[%u]: ", error); // 打印错误信息
  if (error == OTA_AUTH_ERROR) {
    Serial.println("身份验证失败");
  } else if (error == OTA_BEGIN_ERROR) {
    Serial.println("开始更新失败");
  } else if (error == OTA_CONNECT_ERROR) {
    Serial.println("连接失败");
  } else if (error == OTA_RECEIVE_ERROR) {
    Serial.println("接收数据失败");
  } else if (error == OTA_END_ERROR) {
    Serial.println("结束更新失败");
  }
});

ArduinoOTA.begin(); // 开始 OTA 更新

display.init(); // 初始化 OLED 显示屏
display.clear(); // 清空屏幕
display.setFont(ArialMT_Plain_10); // 设置字体
display.flipScreenVertically(); // 垂直翻转屏幕

while (WiFi.status() != WL_CONNECTED) // 等待连接成功
{
  delay(1000); // 延迟 1 秒
  Serial.println("正在连接 WiFi..."); // 输出连接信息
}

Serial.println("WiFi 已经连接"); // 输出连接成功信息
draw_info(); // 调用自定义函数，绘制屏幕信息
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
