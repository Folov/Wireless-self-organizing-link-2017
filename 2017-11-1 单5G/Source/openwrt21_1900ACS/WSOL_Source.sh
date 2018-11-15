#!/bin/ash

# Initialize AP
sleep 8
grep Self_id /etc/profile
if [[ $? -ne 0 ]]; then
	echo "Add env variables."
	echo "export Self_id=21" >> /etc/profile
	echo "export SSID=openwrt21" >> /etc/profile
	echo "export SSID_num=21" >> /etc/profile
	echo "export DEVICE=Linksys_AC1900" >> /etc/profile
	echo "export DEVICE_5G1=radio0" >> /etc/profile
	echo "export DEVICE_5G2=radio2" >> /etc/profile
	echo "export WAN_IP=192.168.12.21" >> /etc/profile
	echo "export LAN_IP=192.168.21.21" >> /etc/profile

	source /etc/profile
fi

mkdir -p /root/bakup
kill `ps | grep hostapd | grep -v grep | awk '{print $1}'`
/root/ap21.sh
echo "AP ESTABLASHED!"

# Information Management and Link Maintenance(IMLM)
echo "IMLM running~"
/root/IMLM_S &
echo "IMLM ok~" # never printo
echo "Open iperf -s"
iperf -s &

# Set route from connecting link
echo "Start ROUTE Process"
while [ 1 ]; do
	# 建立向下的路由（阻塞于tcpsrv_down-arm1900）
	/root/tcpsrv_down-arm3200
	# Sometimes occur "bind err: address in use"
	while [[ $? -ne 0 ]]; do
		kill `ps | grep tcpsrv_down-arm3200 | grep -v grep | awk '{print $1}'`
		sleep 1
		/root/tcpsrv_down-arm3200
	done
	echo "tcpsrv_down DONE!"
	# 修改LPM，配置路由
	/root/link_build_down.sh

	# Send LPM to subnet.
	down_num=`tail -n 1 /tmp/wsol/LPM_GET.txt | head -n 1 | sed 's/openwrt//'`
	/root/tcpcli-arm3200 192.168.$down_num.$down_num /tmp/wsol/LPM_TO_SEND.txt
	# Sometimes occur "Network unreachable"
	while [[ $? -ne 0 ]]; do
		echo "Wait for tcpcli..."
		sleep 1
		/root/tcpcli-arm3200 192.168.$down_num.$down_num /tmp/wsol/LPM_TO_SEND.txt
	done
	echo "tcpcli DONE!"

	# Bacup for route table
	echo "#!/bin/ash" >| /root/bakup/routebak.sh
	route -n | tail -n +3 | awk '{print "route add -net "$1" gw "$2" netmask "$3" dev "$8}' >> /root/bakup/routebak.sh
	echo "ROUTE ESTABLASHED!"
done


