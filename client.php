<?php

error_reporting(E_ALL);
//端口111  
$service_port = 8888;
//本地  
$address = '127.0.0.1';
//创建 TCP/IP socket  
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if ($socket < 0) {
    echo "socket创建失败原因: " . socket_strerror($socket) . "\n";
} 
$result = socket_connect($socket, $address, $service_port);
if ($result < 0) {
    echo "SOCKET连接失败原因: ($result) " . socket_strerror($result) . "\n";
}
//发送命令  
$in = "nihao";

socket_write($socket, $in, strlen($in));
 
while ($out = socket_read($socket, 2048)) {
    echo $out . "\n";
}
 
socket_close($socket);