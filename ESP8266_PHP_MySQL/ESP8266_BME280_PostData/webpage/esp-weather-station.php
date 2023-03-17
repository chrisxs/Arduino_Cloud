<?php
include_once('esp-database.php');
if ($_GET["readingsCount"]) {
    $data = $_GET["readingsCount"];
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    $readings_count = $_GET["readingsCount"];
}
// default readings count set to 20
else {
    $readings_count = 20;
}

$last_reading = getLastReadings();
$last_reading_temp = $last_reading["value1"];
$last_reading_humi = $last_reading["value2"];
$last_reading_time = $last_reading["reading_time"];

// Uncomment to set timezone to - 1 hour (you can change 1 to any number)
//$last_reading_time = date("Y-m-d H:i:s", strtotime("$last_reading_time - 1 hours"));
// Uncomment to set timezone to + 7 hours (you can change 7 to any number)
//$last_reading_time = date("Y-m-d H:i:s", strtotime("$last_reading_time + 7 hours"));

$min_temp = minReading($readings_count, 'value1');
$max_temp = maxReading($readings_count, 'value1');
$avg_temp = avgReading($readings_count, 'value1');

$min_humi = minReading($readings_count, 'value2');
$max_humi = maxReading($readings_count, 'value2');
$avg_humi = avgReading($readings_count, 'value2');
?>

<!DOCTYPE html>
<title>滑稽哥的实验气象站</title>
<html>

<head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">

    <link rel="stylesheet" type="text/css" href="esp-style.css">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <!-- HTTPS则如下 -->
    <!--<script src="http://lib.sinaapp.com/js/jquery/1.9.1/jquery-1.9.1.min.js"></script>-->
    <!-- HTTP则如下 -->
    <script src="http://lib.sinaapp.com/js/jquery/1.9.1/jquery-1.9.1.min.js"></script>
</head>
<header class="header">
    <h1>📊 滑稽哥的实验气象站 🌦️</h1>
    <form method="get">
        <input type="number" name="readingsCount" min="1" placeholder="此处设置记录最大显示数 (<?php echo $readings_count; ?>)条">
        <input type="submit" value="确定">
    </form>
</header>

<body>
    <p>最后读取于:<?php echo $last_reading_time; ?></p>
    <section class="content">
        <div class="box gauge--1">
            <h3>温度</h3>
            <div class="mask">
                <div class="semi-circle"></div>
                <div class="semi-circle--mask"></div>
            </div>
            <p style="font-size: 30px;" id="temp">--</p>
            <table cellspacing="5" cellpadding="5">
                <tr>
                    <th colspan="3">温度每 <?php echo $readings_count; ?> 秒读数</th>
                </tr>
                <tr>
                    <td>最低</td>
                    <td>最高</td>
                    <td>平均</td>
                </tr>
                <tr>
                    <td><?php echo $min_temp['min_amount']; ?> &deg;C</td>
                    <td><?php echo $max_temp['max_amount']; ?> &deg;C</td>
                    <td><?php echo round($avg_temp['avg_amount'], 2); ?> &deg;C</td>
                </tr>
            </table>
        </div>
        <div class="box gauge--2">
            <h3>湿度</h3>
            <div class="mask">
                <div class="semi-circle"></div>
                <div class="semi-circle--mask"></div>
            </div>
            <p style="font-size: 30px;" id="humi">--</p>
            <table cellspacing="5" cellpadding="5">
                <tr>
                    <th colspan="3">湿度每 <?php echo $readings_count; ?> 读数</th>
                </tr>
                <tr>
                    <td>最低</td>
                    <td>最高</td>
                    <td>平均</td>
                </tr>
                <tr>
                    <td><?php echo $min_humi['min_amount']; ?> %</td>
                    <td><?php echo $max_humi['max_amount']; ?> %</td>
                    <td><?php echo round($avg_humi['avg_amount'], 2); ?> %</td>
                </tr>
            </table>
        </div>
    </section>
    <?php
    echo   '<h2> 最后 ' . $readings_count . ' 次读数</h2>
            <table cellspacing="5" cellpadding="5" id="tableReadings">
                <tr>
                    <th>ID</th>
                    <th>传感器</th>
                    <th>位置</th>
                    <th>温度</th>
                    <th>湿度</th>
                    <th>大气压</th>
                    <th>时间戳</th>
                </tr>';

    $result = getAllReadings($readings_count);
    if ($result) {
        while ($row = $result->fetch_assoc()) {
            $row_id = $row["id"];
            $row_sensor = $row["sensor"];
            $row_location = $row["location"];
            $row_value1 = $row["value1"];
            $row_value2 = $row["value2"];
            $row_value3 = $row["value3"];
            $row_reading_time = $row["reading_time"];
            // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
            //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time - 1 hours"));
            // Uncomment to set timezone to + 7 hours (you can change 7 to any number)
            //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 7 hours"));

            echo '<tr>
                    <td>' . $row_id . '</td>
                    <td>' . $row_sensor . '</td>
                    <td>' . $row_location . '</td>
                    <td>' . $row_value1 . '</td>
                    <td>' . $row_value2 . '</td>
                    <td>' . $row_value3 . '</td>
                    <td>' . $row_reading_time . '</td>
                  </tr>';
        }
        echo '</table>';
        $result->free();
    }
    ?>

    <script>
        var value1 = <?php echo $last_reading_temp; ?>;
        var value2 = <?php echo $last_reading_humi; ?>;
        setTemperature(value1);
        setHumidity(value2);

        function setTemperature(curVal) {
            //set range for Temperature in Celsius -5 Celsius to 38 Celsius
            var minTemp = -5.0;
            var maxTemp = 38.0;
            //set range for Temperature in Fahrenheit 23 Fahrenheit to 100 Fahrenheit
            //var minTemp = 23;
            //var maxTemp = 100;

            var newVal = scaleValue(curVal, [minTemp, maxTemp], [0, 180]);
            $('.gauge--1 .semi-circle--mask').attr({
                style: '-webkit-transform: rotate(' + newVal + 'deg);' +
                    '-moz-transform: rotate(' + newVal + 'deg);' +
                    'transform: rotate(' + newVal + 'deg);'
            });
            $("#temp").text(curVal + ' ºC');
        }

        function setHumidity(curVal) {
            //set range for Humidity percentage 0 % to 100 %
            var minHumi = 0;
            var maxHumi = 100;

            var newVal = scaleValue(curVal, [minHumi, maxHumi], [0, 180]);
            $('.gauge--2 .semi-circle--mask').attr({
                style: '-webkit-transform: rotate(' + newVal + 'deg);' +
                    '-moz-transform: rotate(' + newVal + 'deg);' +
                    'transform: rotate(' + newVal + 'deg);'
            });
            $("#humi").text(curVal + ' %');
        }

        function scaleValue(value, from, to) {
            var scale = (to[1] - to[0]) / (from[1] - from[0]);
            var capped = Math.min(from[1], Math.max(from[0], value)) - from[0];
            return ~~(capped * scale + to[0]);
        }
    </script>
</body>

</html>