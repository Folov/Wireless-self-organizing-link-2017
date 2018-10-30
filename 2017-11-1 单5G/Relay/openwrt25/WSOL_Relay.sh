#!/bin/ash

sleep 6
source /etc/profile
grep Self_id /etc/profile
if [[ $? -ne 0 ]]; then
	echo "Add env variables."
	echo "export Self_id=25" >> /etc/profile
	echo "export SSID=openwrt25" >> /etc/profile
	echo "export SSID_num=25" >> /etc/profile
	echo "export DEVICE=Linksys_AC1900" >> /etc/profile
	echo "export DEVICE_5G1=radio0" >> /etc/profile
	echo "export DEVICE_5G2=radio2" >> /etc/profile
	echo "export WAN_IP=192.168.12.25" >> /etc/profile
	echo "export LAN_IP=192.168.25.25" >> /etc/profile

	source /etc/profile
fi

mkdir -p /root/bakup
sleep 2
kill `ps | grep hostapd | grep -v grep | awk '{print $1}'`
/root/ap25.sh
/root/led_config.sh -s sata -t 20 20
/root/led_config.sh -s usb1 -d off

# 建立向下的路由（阻塞于tcpsrv_down-arm1900）
/root/tcpsrv_down-arm1900
# Sometimes occur "bind err: address in use"
while [[ $? -ne 0 ]]; do
	kill `ps | grep tcpsrv_down-arm1900 | grep -v grep | awk '{print $1}'`
	kill `ps | grep tcpsrv_up-arm1900 | grep -v grep | awk '{print $1}'`
	sleep 1
	/root/tcpsrv_down-arm1900
done
echo "tcpsrv_down DONE!"
/root/led_config.sh -s sata -t 60 60

# 修改LPM，配置路由
/root/link_build_down.sh
/root/led_config.sh -s sata -t 150 150

# 建立向上的路由
/root/tcpsrv_up-arm1900 &
sleep 1
# Sometimes occur "bind err: address in use"
pid_tcpsrv=`ps | grep tcpsrv_up-arm1900 | grep -v grep | awk '{print $1}'`
while [ -z "$pid_tcpsrv" ]; do
	echo "Wait for tcpsrv_up &..."
	/root/tcpsrv_up-arm1900 &
	sleep 1
	pid_tcpsrv=`ps | grep tcpsrv_up-arm1900 | grep -v grep | awk '{print $1}'`
done
echo "tcpsrv_up-arm1900 & OPEN!"

/root/findrouter.sh

/root/chooserouter-arm1900 /tmp/wsol/routerlist.txt >| /tmp/wsol/bestrouter.txt
while [[ $? -ne 0 ]]; do
	echo "No matched AP"
	sleep 2
	/root/findrouter.sh
	/root/chooserouter-arm1900 /tmp/wsol/routerlist.txt >| /tmp/wsol/bestrouter.txt
done
echo "bestrouter.txt has been written!"
sleep 1

/root/clientAP_25.sh
sleep 2
/root/led_config.sh -s sata -t 200 300

# Send LPM to up-hop.
nexthop_num=`grep '^ssid' /tmp/wsol/bestrouter.txt | sed -r 's/ssid="openwrt(.*)"/\1/'`
/root/tcpcli-arm1900 192.168.$nexthop_num.$nexthop_num /tmp/wsol/LPM_TO_SEND.txt
# Sometimes occur "Network unreachable"
while [[ $? -ne 0 ]]; do
	echo "Wait for tcpcli..."
	sleep 1
	/root/tcpcli-arm1900 192.168.$nexthop_num.$nexthop_num /tmp/wsol/LPM_TO_SEND.txt
done
echo "tcpcli DONE!"
/root/led_config.sh -s usb1 -t 20 20

# After getting data from up-hop, tcpsrv_up-arm1900 call link_build_up.sh by system()
# link_build_up.sh will send LPM_TOP.txt to subnet by tcpcli-arm1900

# Wait for tcpsrv_up-arm1900 end.
ps | grep tcpsrv_up-arm1900 | grep -v grep
while [[ $? -eq 0 ]]; do
	sleep 1
	ps | grep tcpsrv_up-arm1900 | grep -v grep
done
echo "tcpsrv_up-arm1900 & DONE!"
/root/led_config.sh -s usb1 -t 60 60

# Bacup for bestrouter to client
cp /tmp/wsol/bestrouter.txt /root/bakup/bestrouter.bak
# Bacup for route table
echo "#!/bin/ash" >| /root/bakup/routebak.sh
route -n | tail -n +3 | awk '{print "route add -net "$1" gw "$2" netmask "$3" dev "$8}' >> /root/bakup/routebak.sh
echo "WSOL ESTABLASHED!"
/root/led_config.sh -s usb1 -t 150 150

# Information Management and Link Maintenance(IMLM)
echo "IMLM running~"
sleep 2
/root/iwscan_trans.sh
sourcehop_num=`tail -n 1 /tmp/wsol/LPM_TOP.txt | sed 's/openwrt//'`
/root/led_config.sh -s sata -d on
/root/led_config.sh -s usb1 -d on

/root/IMLM_RU 192.168.$sourcehop_num.$sourcehop_num $SSID
while [[ $? -eq 0 ]]; do
	sleep 1
	/root/IMLM_RU 192.168.$sourcehop_num.$sourcehop_num $SSID
echo "IMLM ok~" # never print
