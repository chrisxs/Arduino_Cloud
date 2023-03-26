#include <ESP8266WebServer.h>

const char *ssid = "ESP8266AP";
const char *password = "";

ESP8266WebServer server(80);

void connectWiFi();
void handleRoot();
void handleSave();
void handleRestart();
void handleClear();
bool loadConfig();

void handleRoot()
{
  String html = "<html><head>"
                "<meta charset=\"UTF-8\">"
                "<style> body { text-align: center; } </style>"
                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
                "<title>ESP8266 WiFi连接配置页面</title>"
                "<h1>滑稽哥的mini WiFiManager!</h1>"
                "<p>主页：chrisxs.com</p><br><br>"
                "<img src='https://chrisxs.com/web-images/iot.png' style=\"width:20%;\" /><br><br>"
                "<a href='http://"+WiFi.localIP().toString()+":8266/update'>已连接WiFi的，点击此处更新固件</a><br><br>"
                "<a href='http://192.168.4.1:8266/update'>未连接WiFi的，点击此处更新固件</a><br><br>"
                "</head><body>"
                "<form method='post' action='/save'>"
                "<label>WiFi SSID:</label>"
                "<input type='text' name='ssid'><br>"
                "<label>WiFi 密码:</label>"
                "<input type='password' name='password'><br>"
                "<input type='submit' value='保存'><br><br>"
                "<input type='button' value='重启' onclick='restart()'>"
                "<input type='button' value='清除WiFi凭据' onclick='clearCred()'>"
                "</form>"
                "<script>"
                "function restart() {"
                "var xhr = new XMLHttpRequest();"
                "xhr.open(\"POST\", \"/restart\", true);"
                "xhr.onload = function() {"
                "if (xhr.readyState === xhr.DONE) {"
                "if (xhr.status === 200) {"
                "alert('重启成功！');"
                "} else {"
                "alert('重启失败！');"
                "}"
                "}"
                "};"
                "xhr.send();"
                "}"
                "function clearCred() {"
                "var xhr = new XMLHttpRequest();"
                "xhr.open(\"POST\", \"/clear\", true);"
                "xhr.onload = function() {"
                "if (xhr.readyState === xhr.DONE) {"
                "if (xhr.status === 200) {"
                "alert('WiFi凭据清理成功！');"
                "} else {"
                "alert('WiFi凭据清理失败！');"
                "}"
                "}"
                "};"
                "xhr.send();"
                "}"
                "</script>"
                "</body></html>";

  server.send(200, "text/html", html);
}

void handleSave()
{
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  if (server.hasArg("clear"))
  {
    // 如果点击了“Clear WiFi Credentials”按钮，则删除配置文件并重启设备。
    SPIFFS.remove("/config.txt");
    ESP.restart();
  }
  // else if (ssid.length() > 0) //允许连接无密码的WiFi_SSID（不建议，太不安全）
  else if (ssid.length() > 0 && password.length() > 0) // 不允许连接无密码的WiFi_SSID
  {
    // 如果输入了新的SSID和密码，则保存它们并重启设备。
    File configFile = SPIFFS.open("/config.txt", "w");
    if (configFile)
    {
      configFile.println(ssid);
      configFile.println(password);
      configFile.close();
      Serial.println("WiFi凭据保存成功");
      server.send(200, "text/html", "<meta charset=\"UTF-8\"><p>保存成功！！稍后即将重启！</p>");
      delay(3000);
      ESP.restart();
      return;
    }
  }
  // server.send(400, "text/html", "保存失败！！");
  server.send(400, "text/html", "<meta charset=\"UTF-8\"><p>保存失败！！请返回上个页面！</p>");
}

bool loadConfig()
{
  if (SPIFFS.exists("/config.txt"))
  {
    File configFile = SPIFFS.open("/config.txt", "r");
    if (configFile)
    {
      String ssid = configFile.readStringUntil('\n');
      String password = configFile.readStringUntil('\n');
      configFile.close();
      ssid.trim();
      password.trim();
      if (ssid.length() > 0 && password.length() > 0)
      {
        WiFi.begin(ssid.c_str(), password.c_str());
        int tries = 0;
        Serial.print("正在连接  ");
        Serial.println(ssid);
        while (WiFi.status() != WL_CONNECTED && tries < 10)
        {
          delay(1000);
          Serial.print(".");
          tries++;
        }
        if (WiFi.status() == WL_CONNECTED)
        {
          Serial.println("");
          Serial.print("WiFi 已经连接, IP 地址: ");
          Serial.println(WiFi.localIP());
          return true;
        }
        else
        {
          Serial.println("");
          Serial.println("连接 WiFi 失败");
        }
      }
    }
  }
  return false;
}

void connectWiFi()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    return;
  }
  Serial.println("正在连接 WiFi...");
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 10)
  {
    delay(1000);
    Serial.print(".");
    tries++;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.print("WiFi 已经连接, IP 地址: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    WiFi.disconnect();
    delay(1000);
    ESP.restart();
  }
}

void handleRestart()
{
  server.send(200, "text/plain", "Restarting...");
  delay(1000);
  Serial.print("用户点击重启");
  delay(3000);
  ESP.restart();
}

void handleClear()
{
  SPIFFS.remove("/config.txt");
  server.send(200, "text/plain", "Clear Credentials");
  Serial.print("用户清除WiFi凭据");
  delay(3000);
  WiFi.disconnect();
  delay(1000);
  ESP.restart();
}