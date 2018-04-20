#!/bin/ash
# 使用udpcli程序发送自身SSID给源端作为心跳并接收Echo信息
# 连续发送或接收失败(都将导致udpcli返回错误)5次后重启

# min:3*5=15s	max:6*5=30s
SOURCE_IP=192.168.11.11
err_flag=0

while [ 1 ]; do
	/root/udpcli-arm1900 $SOURCE_IP $SSID
	if [[ $? -ne 0 ]]; then
		err_flag=$(expr $err_flag + 1)
	else
		err_flag=0
	fi

	if [[ $err_flag -eq 5 ]]; then
		echo "Source is unreachable. 5 times try failed."
		echo "5s waiting to reboot....."
		sleep 1
		echo "4s waiting to reboot...."
		sleep 1
		echo "3s waiting to reboot..."
		sleep 1
		echo "2s waiting to reboot.."
		sleep 1
		echo "1s waiting to reboot."
		sleep 1
		# reboot
		exit 1
	fi

	sleep 3
done
