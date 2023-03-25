<?php

// 数据库服务器名称
$servername = "你的数据库路径";

// 数据库名称
$dbname = "你的数据库名称";

// 数据库用户名
$username = "你的数据库用户名";

// 数据库用户密码
$password = "你的数据库用户名";

// 此 API Key 值需要与项目页面中提供的 ESP32 代码保持兼容，如果更改此值，ESP32 草稿需要匹配
$api_key_value = "你的数据库API_KEY";

// 初始化变量
$api_key = $value1 = $value2 = $value3 = "";

// 如果是 POST 请求，获取提交的数据
if ($_SERVER["REQUEST_METHOD"] == "POST") {

    // 获取提交的 API Key
    $api_key = test_input($_POST["api_key"]);

    // 检查 API Key 是否正确
    if($api_key == $api_key_value) {

        // 获取提交的 value1, value2 和 value3
        $value1 = test_input($_POST["value1"]);
        $value2 = test_input($_POST["value2"]);
        $value3 = test_input($_POST["value3"]);

        // 创建 MySQL 连接
        $conn = new mysqli($servername, $username, $password, $dbname);

        // 检查连接是否成功
        if ($conn->connect_error) {
            die("连接失败：" . $conn->connect_error);
        } 

        // 创建 SQL 查询语句
        $sql = "INSERT INTO SensorChart (value1, value2, value3)
        VALUES ('" . $value1 . "', '" . $value2 . "', '" . $value3 . "')";

        // 执行 SQL 查询语句
        if ($conn->query($sql) === TRUE) {
            echo "新记录创建成功";
        } 
        else {
            echo "错误：" . $sql . "<br>" . $conn->error;
        }

        // 关闭 MySQL 连接
        $conn->close();
    }
    else {
        echo "提供的 API Key 不正确。";
    }

}
else {
    echo "没有使用 HTTP POST 发布任何数据。";
}

// 函数用于处理提交的数据
function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
