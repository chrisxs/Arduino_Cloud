#include <ElegantOTA.h>

ESP8266WebServer OTAserver(8266);

void OTA();

void OTA()
{
    OTAserver.on("/", []()
              { OTAserver.send(200, "text/plain", "Hi! I am ESP8266."); });

    ElegantOTA.begin(&OTAserver); 
    OTAserver.begin();
    Serial.println("OTA服务启动成功");
}