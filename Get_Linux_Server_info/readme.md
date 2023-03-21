# 使用esp8266+ssd1306液晶获取Linux主机信息

## 更新

2023年3月21日：
- 改成通过WiFiManager输入服务器的域名、token
---
2023年3月20日：
- 上传

## 使用方法
1. 把`sysinfo.php`放到Apache或者Nginx服务器的网页根目录下，并修改里面的token，可以自定义
2. 用浏览器验证，地址栏输入：`https://你的域名/sysinfo.php?token=你的token`。成功的话会返回硬件信息
3. 写入Arduino程序到ESP8266，注意代码中的token要与`sysinfo.php`里面的相配
