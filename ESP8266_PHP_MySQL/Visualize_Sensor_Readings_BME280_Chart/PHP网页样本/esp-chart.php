<?php
// 设置页面标题
$title = "滑稽哥的小实验";
echo "<script>document.title = \"".$title."\" </script>";

// 数据库信息
$servername = "你的数据库路径";
$dbname = "你的数据库名称";
$username = "你的数据库用户名";
$password = "你的数据库用户名";

// 连接数据库
$conn = new mysqli($servername, $username, $password, $dbname);
// 检查连接
if ($conn->connect_error) {
    die("连接失败：" . $conn->connect_error);
} 

// 查询数据库
$sql = "SELECT id, value1, value2, value3, reading_time FROM SensorChart order by reading_time desc limit 40";
$result = $conn->query($sql);

// 保存查询结果
while ($data = $result->fetch_assoc()){
    $sensor_data[] = $data;
}

// 获取读取时间数据
$readings_time = array_column($sensor_data, 'reading_time');

// 转换读取时间数据的时区，如果需要的话，取消下面的注释
/*
$i = 0;
foreach ($readings_time as $reading){
    // 取消注释将时区设置为-1小时（您可以将1更改为任何数字）
    $readings_time[$i] = date("Y-m-d H:i:s", strtotime("$reading - 1 hours"));
    // 取消注释将时区设置为+4小时（您可以将4更改为任何数字）
    //$readings_time[$i] = date("Y-m-d H:i:s", strtotime("$reading + 4 hours"));
    $i += 1;
}
*/

// 获取三个值的数据并反转数组
$value1 = json_encode(array_reverse(array_column($sensor_data, 'value1')), JSON_NUMERIC_CHECK);
$value2 = json_encode(array_reverse(array_column($sensor_data, 'value2')), JSON_NUMERIC_CHECK);
$value3 = json_encode(array_reverse(array_column($sensor_data, 'value3')), JSON_NUMERIC_CHECK);

// 获取反转后的读取时间数据
$reading_time = json_encode(array_reverse($readings_time), JSON_NUMERIC_CHECK);

// 释放查询结果内存并关闭连接
$result->free();
$conn->close();
?>


<!DOCTYPE html>
<html>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta http-equiv="refresh" content="30"> <!--自动每30刷新秒刷新一次-->
  <script src="https://code.highcharts.com/highcharts.js"></script>
  <!--HTTP-->
  <!--<script src="http://code.highcharts.com/highcharts.js"></script>-->

  <style>
    body {
      min-width: 310px;
    	max-width: 1280px;
    	height: 500px;
      margin: 0 auto;
    }
    h2 {
      font-family: Arial;
      font-size: 2.5rem;
      text-align: center;
    }
  </style>
  <body>
    <h2>🌦️滑稽哥的气象站📈</h2>
    <div id="chart-temperature" class="container"></div>
    <div id="chart-humidity" class="container"></div>
    <div id="chart-pressure" class="container"></div>
<script>

var value1 = <?php echo $value1; ?>;
var value2 = <?php echo $value2; ?>;
var value3 = <?php echo $value3; ?>;
var reading_time = <?php echo $reading_time; ?>;

var chartT = new Highcharts.Chart({
  chart:{ renderTo : 'chart-temperature' },
  title: { text: 'BME280 温度走势图 🌡️' },
  series: [{
    showInLegend: false,
    data: value1
  }],
  plotOptions: {
    line: { animation: false,
      dataLabels: { enabled: true }
    },
    series: { color: '#059e8a' }
  },
  xAxis: { 
    type: 'datetime',
    categories: reading_time
  },
  yAxis: {
    title: { text: '温度 (摄氏) 🌡️' }
    //title: { text: '温度 (华氏)' }
  },
  credits: { enabled: false }
});

var chartH = new Highcharts.Chart({
  chart:{ renderTo:'chart-humidity' },
  title: { text: 'BME280 湿度走势图 💧' },
  series: [{
    showInLegend: false,
    data: value2
  }],
  plotOptions: {
    line: { animation: false,
      dataLabels: { enabled: true }
    }
  },
  xAxis: {
    type: 'datetime',
    //dateTimeLabelFormats: { second: '%H:%M:%S' },
    categories: reading_time
  },
  yAxis: {
    title: { text: '湿度 (%) 💧' }
  },
  credits: { enabled: false }
});


var chartP = new Highcharts.Chart({
  chart:{ renderTo:'chart-pressure' },
  title: { text: 'BME280 大气压走势图 ☁️' },
  series: [{
    showInLegend: false,
    data: value3
  }],
  plotOptions: {
    line: { animation: false,
      dataLabels: { enabled: true }
    },
    series: { color: '#18009c' }
  },
  xAxis: {
    type: 'datetime',
    categories: reading_time
  },
  yAxis: {
    title: { text: '大气压 (hPa) ☁️' }
  },
  credits: { enabled: false }
});

</script>
</body>
</html>