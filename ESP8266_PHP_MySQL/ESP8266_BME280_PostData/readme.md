# 使用BME280发送数据到服务器，显示方式为gauge（仪表圆盘），ESP32和ESP8266通用

# 使用方法
1. 新建数据库，名为`espdb`

2. 使用`SensorData_Table.sql`，初始化数据库表

3. 把PHP网页样本文件夹里的目录放到Apache或者Nginx的网页目录里，并设置775权限。

4. 修改`esp-database.php`中的：
- `$servername`：MySQL服务器的主机名或IP地址。
- `$dbname`：MySQL数据库的名称。
- `$username`：MySQL数据库的用户名。
- `$password`：MySQL数据库的密码。

5. 修改`esp-post-data.php`中的：`$api_key_value = "你的数据库API_KEY";`，要与Arduino代码中的`String apiKeyValue = "你的数据库API_KEY";`相符，否则无法访问数据库。

6. 修改Arduino代码中的WiFi信息和服务器路径，写入arduino程序

# 更新日志
2023年3月22日
- 修改注释
- 精简代码