<?php
$servername = '你的数据库host名称';

$dbname = '你的数据库名称';
$username = '你的数据库用户名';
$password = '你的数据库密码';

function insertReading( $sensor, $location, $value1, $value2, $value3 ) {
    global $servername, $username, $password, $dbname;

    // 创建连接
    $conn = new mysqli( $servername, $username, $password, $dbname );
    // 检查连接
    if ( $conn->connect_error ) {
        die( 'Connection failed: ' . $conn->connect_error );
    }

    // SQL 插入语句
    $sql = "INSERT INTO sensordata (sensor, location, value1, value2, value3)
    VALUES ('" . $sensor . "', '" . $location . "', '" . $value1 . "', '" . $value2 . "', '" . $value3 . "')";

    // 执行 SQL 语句并检查结果
    if ( $conn->query( $sql ) === TRUE ) {
        return 'New record created successfully';
    } else {
        return 'Error: ' . $sql . '<br>' . $conn->error;
    }

    // 关闭连接
    $conn->close();
}

function getAllReadings( $limit ) {
    global $servername, $username, $password, $dbname;
    // 定义全局变量，包括数据库服务器名、用户名、密码和数据库名

    // 创建数据库连接
    $conn = new mysqli( $servername, $username, $password, $dbname );
    // 检查数据库连接是否成功
    if ( $conn->connect_error ) {
        die( 'Connection failed: ' . $conn->connect_error );
        // 如果连接失败，输出错误信息并终止脚本
    }

    // 构造查询语句，查询表 sensordata 中的指定字段（id、sensor、location、value1、value2、value3 和 reading_time），按照 reading_time 字段的降序排序，最多返回 $limit 条记录
    $sql = 'SELECT id, sensor, location, value1, value2, value3, reading_time FROM sensordata order by reading_time desc limit ' . $limit;
    if ( $result = $conn->query( $sql ) ) {
        return $result;
        // 如果查询成功，返回查询结果集
    } else {
        return false;
        // 如果查询失败，返回 false
    }
    $conn->close();
    // 关闭数据库连接
}

function getLastReadings() {
    global $servername, $username, $password, $dbname;

    // 创建连接
    $conn = new mysqli( $servername, $username, $password, $dbname );
    // 检查连接是否成功
    if ( $conn->connect_error ) {
        die( 'Connection failed: ' . $conn->connect_error );
    }

    // 查询最近一次传感器数据
    $sql = 'SELECT id, sensor, location, value1, value2, value3, reading_time FROM sensordata order by reading_time desc limit 1';
    if ( $result = $conn->query( $sql ) ) {
        return $result->fetch_assoc();
        // 返回查询结果
    } else {
        return false;
        // 返回失败标识
    }

    $conn->close();
    // 关闭连接
}

function minReading( $limit, $value ) {
    global $servername, $username, $password, $dbname;

    // 创建数据库连接
    $conn = new mysqli( $servername, $username, $password, $dbname );
    // 检查连接是否成功
    if ( $conn->connect_error ) {
        die( '连接失败：' . $conn->connect_error );
    }

    // 构建 SQL 查询语句，查询最小值
    $sql = 'SELECT MIN(' . $value . ') AS min_amount FROM (SELECT ' . $value . ' FROM sensordata order by reading_time desc limit ' . $limit . ') AS min';
    if ( $result = $conn->query( $sql ) ) {
        // 获取查询结果，并返回最小值
        return $result->fetch_assoc();
    } else {
        // 查询失败，返回false
        return false;
    }

    // 关闭数据库连接
    $conn->close();
}

function maxReading( $limit, $value ) {
    global $servername, $username, $password, $dbname;

    // 创建连接
    $conn = new mysqli( $servername, $username, $password, $dbname );
    // 检查连接是否成功
    if ( $conn->connect_error ) {
        die( 'Connection failed: ' . $conn->connect_error );
    }

    // 从sensordata表中按照reading_time降序排列，取前$limit条数据中$value的最大值
    $sql = 'SELECT MAX(' . $value . ') AS max_amount FROM (SELECT ' . $value . ' FROM sensordata order by reading_time desc limit ' . $limit . ') AS max';
    if ( $result = $conn->query( $sql ) ) {
        // 返回查询结果的关联数组
        return $result->fetch_assoc();
    } else {
        // 如果查询失败，返回false
        return false;
    }
    // 关闭连接
    $conn->close();
}

function avgReading( $limit, $value ) {
    global $servername, $username, $password, $dbname;

    // 创建连接
    $conn = new mysqli( $servername, $username, $password, $dbname );
    // 检查连接是否成功
    if ( $conn->connect_error ) {
        die( 'Connection failed: ' . $conn->connect_error );
    }

    // 查询最近 $limit 条记录中 $value 字段的平均值
    $sql = 'SELECT AVG(' . $value . ') AS avg_amount FROM (SELECT ' . $value . ' FROM sensordata order by reading_time desc limit ' . $limit . ') AS avg';
    if ( $result = $conn->query( $sql ) ) {
        return $result->fetch_assoc();
    } else {
        return false;
    }
    $conn->close();
}

?>