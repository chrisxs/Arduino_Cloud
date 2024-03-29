#include <Wire.h>        // 引入I2C库
#include "SSD1306Wire.h" // 引入OLED驱动库


std::string serverUrl_1; // 服务器地址1
std::string serverUrl_2; // 服务器地址2
std::string token; // 令牌

const int SetPin = D3; // 设置引脚号，常量值为D3
bool shouldSaveConfig = false; // 是否应该保存配置

// 初始化OLED对象
SSD1306Wire display(0x3c, D2, D1);

void draw_countdown(); // 定义函数，用于绘制倒计时界面
void draw_info();      // 定义函数，用于绘制信息界面
void draw_note();      // 定义函数，用于绘制提示信息界面
void draw_done();      // 定义函数，用于绘制完成界面

// 定义Wi-Fi图标的数组
const uint8_t wifi_logo[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xE0, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
  0xFF, 0x03, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0xFF, 0xFF, 0xFF, 0x07, 0xC0, 0x83, 0x01, 0x80, 0xFF, 0xFF, 0xFF,
  0x01, 0x00, 0x07, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x0C, 0x00,
  0xC0, 0xFF, 0xFF, 0x7C, 0x00, 0x60, 0x0C, 0x00, 0xC0, 0x31, 0x46, 0x7C,
  0xFC, 0x77, 0x08, 0x00, 0xE0, 0x23, 0xC6, 0x3C, 0xFC, 0x67, 0x18, 0x00,
  0xE0, 0x23, 0xE4, 0x3F, 0x1C, 0x00, 0x18, 0x00, 0xE0, 0x23, 0x60, 0x3C,
  0x1C, 0x70, 0x18, 0x00, 0xE0, 0x03, 0x60, 0x3C, 0x1C, 0x70, 0x18, 0x00,
  0xE0, 0x07, 0x60, 0x3C, 0xFC, 0x73, 0x18, 0x00, 0xE0, 0x87, 0x70, 0x3C,
  0xFC, 0x73, 0x18, 0x00, 0xE0, 0x87, 0x70, 0x3C, 0x1C, 0x70, 0x18, 0x00,
  0xE0, 0x87, 0x70, 0x3C, 0x1C, 0x70, 0x18, 0x00, 0xE0, 0x8F, 0x71, 0x3C,
  0x1C, 0x70, 0x18, 0x00, 0xC0, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x08, 0x00,
  0xC0, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x0C, 0x00, 0x80, 0xFF, 0xFF, 0x1F,
  0x00, 0x00, 0x06, 0x00, 0x80, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x07, 0x00,
  0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0xF8, 0xFF, 0xFF,
  0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00,
  0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x1F, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void draw_info(){
  display.clear();                           // 清空屏幕
  display.setTextAlignment(TEXT_ALIGN_LEFT); // 文字居左对齐
  display.drawXbm(35, 0, 60, 36, wifi_logo); // 在屏幕上绘制WiFi图标
  display.drawString(0, 40,"IP: " +String(WiFi.localIP().toString()));
  display.display();                         // 显示WiFi图标
  delay(3000);
  display.setFont(ArialMT_Plain_24);     // 设置字体大小
  display.clear();                       // 清空屏幕
  display.drawString(33, 5, "Server");   // 在屏幕上显示 "Server" 字符串
  display.drawString(28, 25, "Monitor"); // 在屏幕上显示 "Monitor" 字符串
  display.display();                     // 更新显示内容*/
}

void draw_countdown(){
    display.setFont(ArialMT_Plain_16); // 设置显示字体
    display.clear(); // 清空显示器上的内容
    display.drawString(5, 25, "Reboot in 5 Sec !"); // 在显示器上显示倒计时信息
    display.display(); // 将内容显示到显示器上
    delay(1000); // 程序暂停1秒钟

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
    delay(1000);
}


void draw_note(){    
    display.clear(); // 清空屏幕
    display.drawXbm(35, 0, 60, 36, wifi_logo); // 在屏幕上绘制WiFi图标
    display.setFont(ArialMT_Plain_16); // 设置字体大小
    display.drawString(33, 35, "AP Mode"); // 在屏幕上绘制"AP Mode"文字
    display.setFont(ArialMT_Plain_10); // 设置字体大小
    display.drawString(17, 50, "waiting for config WiFi"); // 在屏幕上绘制"waiting for config WiFi"文字
    display.display(); // 刷新屏幕
    delay(1000); // 延迟1秒
}

void draw_done(){    
    display.clear(); // 清空屏幕
    display.drawXbm(35, 0, 60, 36, wifi_logo); // 在屏幕上绘制WiFi图标
    display.setFont(ArialMT_Plain_16); // 设置字体大小
    display.drawString(45, 35, "Dnoe"); // 在屏幕上绘制"Dnoe"文字
    display.setFont(ArialMT_Plain_10); // 设置字体大小
    display.drawString(17, 50, "reboot for connet WiFi"); // 在屏幕上绘制"reboot for connet WiFi"文字
    display.display(); // 刷新屏幕
    delay(1000); // 延迟1秒
}
