#!/bin/ash

# Initialize AP
sleep 8
grep Self_id /etc/profile
if [[ $? -ne 0 ]]; then
	echo "Add env variables."
	echo "export Self_id=11" >> /etc/profile
	echo "export SSID=openwrt11" >> /etc/profile
	echo "export SSID_num=11" >> /etc/profile
	echo "export DEVICE=Linksys_AC3200" >> /etc/profile
	echo "export DEVICE_5G1=radio0" >> /etc/profile
	echo "export DEVICE_5G2=radio2" >> /etc/profile
	echo "export WAN_IP=192.168.12.11" >> /etc/profile
	echo "export LAN_IP=192.168.11.11" >> /etc/profile

	source /etc/profile
fi

mkdir -p /root/bakup
kill `ps | grep hostapd | grep -v grep | awk '{print $1}'`
/root/ap11.sh
echo "AP ESTABLASHED!"

# Information Management and Link Maintenance(IMLM)
echo "IMLM running~"
/root/IMLM_S &
echo "IMLM ok~" # never printo

# Set route from connecting link
echo "Start ROUTE Process"
while [ 1 ]; do
	/root/WSOL_Route.sh
	echo "ROUTE ESTABLASHED!"
	# For lind_del.sh to pick up MAC
	sleep 2
	/root/findrouter.sh
done


