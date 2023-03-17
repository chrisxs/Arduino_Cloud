<?php
  $token = "密钥";

  // 检查 token 是否存在
  if (!isset($_GET['token'])) {
    http_response_code(401); // Unauthorized
    exit;
  }

  // 检查 token 是否匹配
  $request_token = $_GET['token'];
  if ($request_token !== $token) {
    http_response_code(401); // Unauthorized
    exit;
  }

//$cpu_usage = shell_exec("top -bn1 | grep load | awk '{printf \"%.2f\", $(NF-2)}'");
  $cpu_usage = shell_exec("top -bn1 | grep load | awk '{printf \"%.2f%%\", $(NF-2)*100/4}'");
  $mem_usage = shell_exec("free -m | awk 'NR==2{printf \"%.2f%%\", $3*100/$2 }'");
  $swap_usage = shell_exec("free -m | awk '/Swap/ {printf \"%.2f%%\", \$3*100/\$2}'");
//$rx_rate = shell_exec("ifconfig eth0 | awk '/RX packets/{printf \"%.2f\",$5/1024}'");
//$tx_rate = shell_exec("ifconfig eth0 | awk '/TX packets/{printf \"%.2f\",$5/1024}'");
  echo "CPU Usage: $cpu_usage\n";
  echo "Memory Usage: $mem_usage\n";
  echo "Swap Usage: $swap_usage\n";
//echo "RX Rate: $rx_rate KB/s\n";
//echo "TX Rate: $tx_rate KB/s\n";

//cat获取 eth0 网络接口的 RX 和 TX 速率
  $eth0_rx = shell_exec("cat /proc/net/dev | awk '/eth0/ {print \$2}'");
  $eth0_tx = shell_exec("cat /proc/net/dev | awk '/eth0/ {print \$10}'");

  // 计算 RX 和 TX 的速率（以 MB/s 为单位）
  $eth0_rx_rate = sprintf("%.3f", $eth0_rx / 102400000000 / 3);
  $eth0_tx_rate = sprintf("%.3f", $eth0_tx / 102400000000 );

/*  //ifstat获取 eth0 网络接口的 RX 和 TX 速率
$eth0_rx_rate = shell_exec("ifstat -i eth0 -q 1 1 | awk '/eth0/ {print $6}'");
$eth0_tx_rate = shell_exec("ifstat -i eth0 -q 1 1 | awk '/eth0/ {print $8}'");

// 去除末尾的换行符
$eth0_rx_rate = rtrim($eth0_rx_rate, "\n");
$eth0_tx_rate = rtrim($eth0_tx_rate, "\n");*/

//输出结果分两行
echo "Rx: $eth0_rx_rate MB/s\n";
echo "Tx: $eth0_tx_rate MB/s\n";


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
