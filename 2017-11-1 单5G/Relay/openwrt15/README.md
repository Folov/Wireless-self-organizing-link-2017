# 适用设备
Linksys WRT1900AC
# 参数
LAN_IP=192.168.15.15
WAN_IP=192.168.12.15
DEVICE_5G1=radio0

# 设备类型
中继端R

# ~/目录文件
- bakup文件夹(空)
- ap15.sh
- chooserouter-arm1900
- clientAP_15.sh
- findrouter.sh
- hostapd-wlan10.conf *根据不同硬件适当修改*
- link_build_down.sh
- link_build_up.sh
- reconnect.sh
- tcpcli-arm1900
- tcpsrv_down-arm1900
- tcpsrv_up-arm1900
- WSOL_Relay.sh

# 运行生成文件
1. /tmp/wsol/bestrouter.txt 		(最佳路由)
2. /tmp/wsol/hostapd-wlan10.pid 	(hostapd进程号)
3. /tmp/wsol/iwscan.txt 			(iw scan的详细信息)
4. /tmp/wsol/LPM_GET.txt 			(由下级传入的LPM信息)
5. /tmp/wsol/LPM_TO_SEND.txt 		(将要传给上级的LPM信息)
6. /tmp/wsol/LPM_TOP.txt 			(S端传来的LPM汇总)
7. /tmp/wsol/routerlist.txt 		(iwscan.txt的筛选)
8. /root/bakup/bestrouter.bak 			(最佳路由备份)
9. /root/bakup/routebak.sh 				(路由表备份)