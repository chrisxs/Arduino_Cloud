<?php
// 引入数据库操作相关的代码
include_once('esp-database.php');

// 请保留该API Key值以与项目页面提供的ESP代码兼容。如果更改此值，则需要相应修改ESP代码。
$api_key_value = "你的数据库API_KEY";

$api_key = $sensor = $location = $value1 = $value2 = $value3 = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") { // 检查请求的HTTP方法是否为POST
  $api_key = test_input($_POST["api_key"]); // 从POST请求中获取API Key参数，并通过函数test_input()进行过滤
  if ($api_key == $api_key_value) { // 如果获取到的API Key与预设的相符，则继续处理数据
    $sensor = test_input($_POST["sensor"]); // 从POST请求中获取sensor参数，并通过函数test_input()进行过滤
    $location = test_input($_POST["location"]); // 从POST请求中获取location参数，并通过函数test_input()进行过滤
    $value1 = test_input($_POST["value1"]); // 从POST请求中获取value1参数，并通过函数test_input()进行过滤
    $value2 = test_input($_POST["value2"]); // 从POST请求中获取value2参数，并通过函数test_input()进行过滤
    $value3 = test_input($_POST["value3"]); // 从POST请求中获取value3参数，并通过函数test_input()进行过滤

    $result = insertReading($sensor, $location, $value1, $value2, $value3); // 调用函数insertReading()，将过滤后的参数插入到数据库中，并获取返回结果
    echo $result; // 输出结果
  } else { // 如果获取到的API Key与预设的不符，则提示错误信息
    echo "Wrong API Key provided.";
  }
} else { // 如果请求的HTTP方法不是POST，则提示错误信息
  echo "No data posted with HTTP POST.";
}

function test_input($data) // 定义函数test_input()，用于过滤输入的数据
{
  $data = trim($data); // 去除字符串两侧的空格
  $data = stripslashes($data); // 去除字符串中的反斜杠
  $data = htmlspecialchars($data); // 将字符串中的HTML字符转换为实体
  return $data; // 返回过滤后的数据
}
