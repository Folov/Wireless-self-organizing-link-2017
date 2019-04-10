#!/bin/ash

# 该脚本被Pre_config.sh调用
# 用于恢复原始设置，重新建链

/root/led_config.sh -s sata -t 500 500
/root/RESTART.sh
rm -f /tmp/button.first 
sleep 1
/root/WSOL_User.sh | socat -b 1024 - udp-connect:192.168.12.200:20000 &
