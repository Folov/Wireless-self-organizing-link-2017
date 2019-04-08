#!/bin/ash

# 该脚本被Pre_config.sh调用
# 用于检查WSOL进程是否已经工作，屏蔽重复按键

grep Self_id /etc/profile
if [[ $? -ne 0 ]]; then
	/root/WSOL_User.sh | socat - udp-connect:192.168.12.200:20000 &
fi
