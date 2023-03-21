# 使用esp8266+ssd1306液晶获取Linux主机信息

## 使用方法

1. 把`sysinfo.php`放到Apache或者Nginx服务器的网页根目录下，并修改里面的token，可以自定义

2. 用浏览器验证token，地址栏输入：`https://你的域名/sysinfo.php?token=你的token`。成功的话会返回硬件信息

3. ~~写入Arduino程序到ESP8266，注意代码中的token要与`sysinfo.php`里面的相配~~Arduino代码预留两个服务器URL的文本输入框位置用。

4. 上传代码后第一次开机先做一次恢复设置：
    
    4.1. 开机前按着按钮，通电
    
    4.2. 出现带有WiFi图标的提示后，等待重置即可

    - 如果仍然开不了机，并且串口查看信息是不断重复开机码的，可以试试以下代码，清理eeprom。（之前用过WiFiManager没清理干净WiFi记录的可能会有这情况）

```
/*
EEPROM Clear

Sets all of the bytes of the EEPROM to 0.
This example code is in the public domain.

*/

#include <EEPROM.h>

void setup() {
EEPROM.begin(512);
// write a 0 to all 512 bytes of the EEPROM
for (int i = 0; i < 512; i++) { EEPROM.write(i, 0); }

// turn the LED on when we're done
pinMode(13, OUTPUT);
digitalWrite(13, HIGH);
EEPROM.end();
}

void loop() {}
```


5. 完成以上操作后，开机，打开手机可以看见一个名为`Get_Linux_Server_Info`的热点，点击`config wifi`进入后按着项目填写，然后按`save`即可。

6. 配置好后，会出现提示，等待重启即可。


## 更新日志

2023年3月21日：
- 改成通过WiFiManager输入服务器的域名、token
---
2023年3月20日：
- 上传
