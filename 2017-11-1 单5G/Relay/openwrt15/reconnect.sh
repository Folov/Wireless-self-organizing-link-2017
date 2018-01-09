#!/bin/ash

Self_id=15
/root/ap$Self_id.sh

wpa_supplicant -B -iwlan20 -c /root/bakup/bestrouter.bak
gateway_ip=`grep '^ssid' /root/bakup/bestrouter.bak | sed -r 's/ssid="openwrt(.*)"/\1/'`
sleep 1
ifconfig wlan20 192.168.$gateway_ip.$Self_id netmask 255.255.255.0

chmod +x /root/bakup/routebak.sh
/root/bakup/routebak.sh
