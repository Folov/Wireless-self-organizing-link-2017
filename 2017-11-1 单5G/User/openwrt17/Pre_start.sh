#!/bin/ash

# 该脚本被Pre_config.sh调用
# 用于检查WSOL进程是否已经工作，屏蔽重复按键

if [ -f "/tmp/button.first" ]; then
	exit 1
else
	touch /tmp/button.first	# flag used in Pre_start.sh
	/root/WSOL_User.sh | socat -b 1024 - udp-connect:192.168.12.200:20000 &
fi
