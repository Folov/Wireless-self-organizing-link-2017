# 适用设备
Linksys WRT3200
# 参数
LAN_IP=192.168.11.11
WAN_IP=192.168.12.11
DEVICE_5G=radio0

# 设备类型
源端S
 
# ~/目录文件
- bakup文件夹(空)
- ap11.sh
- hostapd-wlan10.conf *根据不同硬件适当修改*
- link_build_down.sh
- tcpcli-arm3200
- tcpsrv_down-arm3200
- WSOL_Source.sh

# 运行生成文件
1. /tmp/wsol/hostapd-wlan10.pid 	(hostapd进程号)
2. /tmp/wsol/LPM_GET.txt 			(由下级传入的LPM信息)
3. /tmp/wsol/LPM_TO_SEND.txt 		(传给下级的LPM信息)
4. /root/bakup/routebak.sh 				(路由表备份)