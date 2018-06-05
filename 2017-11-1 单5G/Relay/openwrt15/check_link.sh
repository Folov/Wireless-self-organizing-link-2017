#!/bin/ash
# 该脚本用于crontab中定时检查连接状态，有错误的情况下写入errlog文件
# 用于发现错误并作出错误处理

to_num=`cat /tmp/wsol/LPM_TOP.txt | tail -1 | sed 's/openwrt//'`
if [[ -z $to_num ]]; then
	echo "NO /tmp/LPM_TOP.txt!" >> /root/errlog
fi
ping 192.168.$to_num.$to_num -c 3
if [[ $? -ne 0 ]]; then
	echo "PING SOURCE ERR! LINK ERR! READY TO REBOOT." >> /root/errlog
#	reboot
fi

