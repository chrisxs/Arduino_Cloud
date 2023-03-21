<?php
// 定义 API Token
$token = "你的token";

// 检查 token 是否存在
if (!isset($_GET['token'])) {
  // 返回 401 错误码并退出
  http_response_code(401); 
  exit;
}

// 检查 token 是否匹配
$request_token = $_GET['token'];
if ($request_token !== $token) {
  // 返回 401 错误码并退出
  http_response_code(401); 
  exit;
}

// 获取 CPU、内存、交换空间使用率
$cpu_usage = shell_exec("top -bn1 | grep load | awk '{printf \"%.2f%%\", $(NF-2)*100/4}'");
$mem_usage = shell_exec("free -m | awk 'NR==2{printf \"%.2f%%\", $3*100/$2 }'");
$swap_usage = shell_exec("free -m | awk '/Swap/ {printf \"%.2f%%\", \$3*100/\$2}'");

// 输出服务器名称
echo "服务器名称：chrisxs.xyz \n";

// 输出 CPU、内存、交换空间使用率
echo "CPU Usage: $cpu_usage\n";
echo "Memory Usage: $mem_usage\n";
echo "Swap Usage: $swap_usage\n";

/*// 获取 eth0 网络接口的 RX 和 TX 速率
$net_dev = file("/proc/net/dev");
$eth0_info = preg_split('/\s+/', trim($net_dev[2]));
$eth0_rx = $eth0_info[1];
$eth0_tx = $eth0_info[9];

// 计算 RX 和 TX 的速率（以 MB/s 为单位）
$eth0_rx_rate = sprintf("%.3f", $eth0_rx / 1024 / 1024  /100);
$eth0_tx_rate = sprintf("%.3f", $eth0_tx / 1024 / 1024 /100* 3);
  
// 输出 RX 和 TX 的速率
echo "Rx: $eth0_rx_rate Kb/s\n";
echo "Tx: $eth0_tx_rate Kb/s\n";*/



//输出结果
//echo "R: $eth0_rx_rate MB/s T: $eth0_tx_rate MB/s";

/* 打印结果
#Serial.print("CPU Usage: ");
#Serial.println($cpu_usage);
Serial.print("Memory Usage: ");
Serial.println($mem_usage);
Serial.print("Swap Usage: ");
Serial.println($swap_usage);
Serial.print("eth0 RX rate: ");
Serial.println($eth0_rx_rate);
Serial.print("eth0 TX rate: ");
Serial.println($eth0_tx_rate);*/
?>
