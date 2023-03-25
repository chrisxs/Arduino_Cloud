CREATE TABLE SensorChart (
    id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    -- 定义 id 列为整数类型，自动递增并作为主键
    value1 VARCHAR(10),
    -- 定义 value1 列为可存储最长 10 个字符的字符串类型
    value2 VARCHAR(10),
    -- 定义 value2 列为可存储最长 10 个字符的字符串类型
    value3 VARCHAR(10),
    -- 定义 value3 列为可存储最长 10 个字符的字符串类型
    reading_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP -- 定义 reading_time 列为时间戳类型，设置默认值为当前时间，每次更新时自动更新时间戳
);