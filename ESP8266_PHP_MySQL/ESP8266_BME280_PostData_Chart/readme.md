# 使用BME280发送数据到服务器，显示方式为chart(曲线图表)，ESP32和ESP8266通用

# 使用方法
1. 新建数据库，名为`espdb_chart`

2. 使用`SensorChart.sql`，初始化数据库表

3. 把PHP网页样本文件夹里的目录放到Apache或者Nginx的网页目录里，并设置775权限。

4. 修改`esp-database.php`和`esp-chart.php`中的：
- `$servername`：MySQL服务器的主机名或IP地址。
- `$dbname`：MySQL数据库的名称。
- `$username`：MySQL数据库的用户名。
- `$password`：MySQL数据库的密码。
- 修改`esp-chart.php`中的：`$api_key_value = "你的数据库API_KEY";`，要与Arduino代码中的`String apiKeyValue = "你的数据库API_KEY";`相符，否则无法访问数据库。

6. 修改Arduino代码中的WiFi信息和服务器路径，写入arduino程序

## OTA方法：
1. 在代码上传过并成功连接WiFi后，编译程序
2. 在显示屏或者串口查看本机IP，浏览器输入该IP地址：`ip地址:8266/update`,选择`Firmware`然后选择文件
3. 然后在项目目录路径下的：`.pio\build\d1_mini`找到`firmware.bin`二进制文件即可

## WEB串口：
- 在显示屏或者串口查看本机IP，浏览器输入该IP地址：`ip地址:340/webserial`

## 使用库：

```
Platform espressif8266 @ 2.6.3 (required: espressif8266)
├── framework-arduinoespressif8266 @ 3.20704.0 (required: platformio/framework-arduinoespressif8266 @ ~3.20704.0)
├── tool-esptool @ 1.413.0 (required: platformio/tool-esptool @ <2)
├── tool-esptoolpy @ 1.30000.201119 (required: platformio/tool-esptoolpy @ ~1.30000.0)
├── tool-mklittlefs @ 1.203.210628 (required: platformio/tool-mklittlefs @ ~1.203.0)
├── tool-mkspiffs @ 1.200.0 (required: platformio/tool-mkspiffs @ ~1.200.0)
└── toolchain-xtensa @ 2.40802.200502 (required: platformio/toolchain-xtensa @ ~2.40802.0)

Libraries
├── Adafruit BME280 Library @ 2.2.2 (required: adafruit/Adafruit BME280 Library)
│   ├── Adafruit BusIO @ 1.14.1 (required: Adafruit BusIO)
│   └── Adafruit Unified Sensor @ 1.1.9 (required: Adafruit Unified Sensor)
├── AsyncElegantOTA @ 2.2.7 (required: ayushsharma82/AsyncElegantOTA)
├── ESP Async WebServer @ 1.2.3 (required: ESP Async WebServer)
│   ├── AsyncTCP @ 1.1.1 (required: AsyncTCP)
│   └── ESPAsyncTCP @ 1.2.2 (required: ESPAsyncTCP)
└── WebSerial @ 1.3.0 (required: ayushsharma82/WebSerial)
```

# 更新日志
2023年3月24日：
- 增加WEB OTA功能
- 增加WEB 串口
---
2023年3月22日
- 修改注释
- 精简代码
