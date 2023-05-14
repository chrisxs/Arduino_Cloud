//////////////////////////////////////////////还没做好///////////////////
/*#include <ESPConnect.h>

AsyncWebServer wifi_server(88);

void wifi_setting();

void wifi_setting()
{
    ESPConnect.autoConnect(false);
    if (ESPConnect.begin(&wifi_server))
    {
        Serial.println("Connected to WiFi");
        Serial.println("IPAddress: " + WiFi.localIP().toString());
    }
    else
    {
        Serial.println("Failed to connect to WiFi");
    }

    wifi_server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request)
                   { request->send(200, "text/plain", "Hello from ESP"); });

    wifi_server.begin();
}*/