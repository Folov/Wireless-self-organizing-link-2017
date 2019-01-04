#!/bin/ash
# 该脚本用于crontab中定时检查路由器当前状态，有错误的情况下写入errlog文件并重启WSOL程序
# 用于发现错误并作出错误处理

if [[ -e /tmp/RESTART_flag ]]; then
	rm -f /tmp/RESTART_flag
	/root/WSOL_Relay.sh > /tmp/WSOL_Relay.out 2>&1 &
fi

# pgrep hostapd
# if [[ $? -ne 0 ]]; then
# 	echo "hostapd ERR." >> /root/errlog
# 	/root/RESTART.sh
# #	reboot
# fi

# pgrep wpa_supplicant
# if [[ $? -ne 0 ]]; then
# 	echo "wpa_supplicant ERR." >> /root/errlog
# 	/root/RESTART.sh
# #	reboot
# fi

