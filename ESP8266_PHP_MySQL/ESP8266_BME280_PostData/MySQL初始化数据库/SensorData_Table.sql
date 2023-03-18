CREATE TABLE sensordata (
    id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    -- 数据库主键
    sensor VARCHAR(30) NOT NULL,
    -- 传感器名称
    location VARCHAR(30) NOT NULL,
    -- 传感器位置
    value1 VARCHAR(10),
    -- 第一个传感器值
    value2 VARCHAR(10),
    -- 第二个传感器值
    value3 VARCHAR(10),
    -- 第三个传感器值
    reading_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP -- 数据采集时间戳
);