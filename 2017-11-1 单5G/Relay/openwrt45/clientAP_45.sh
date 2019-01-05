#!/bin/ash
echo "clientAP_45"
# This file is to client given AP by 'bestrouter.txt'
# By use of wpa_supplicant.
# And then, ifconfig ip and netmask, config route table.

ps | grep wpa_supplicant | grep -v grep
if [ $? -ne 0 ]
then
	wpa_supplicant -B -iwlan20 -c /tmp/wsol/bestrouter.txt
else
	# kill `cat /tmp/wsol/bestrouter.pid`
	kill `ps | grep wpa_supplicant | grep -v grep | awk '{print $1}'`
	wpa_supplicant -B -iwlan20 -c /tmp/wsol/bestrouter.txt
fi
sleep 1

gateway_ip=`grep '^ssid' /tmp/wsol/bestrouter.txt | sed -r 's/ssid="openwrt(.*)"/\1/'`

ifconfig wlan20 192.168.$gateway_ip.$Self_id netmask 255.255.255.0
sleep 1
route add -net 192.168.$gateway_ip.0 netmask 255.255.255.0 gw 192.168.$gateway_ip.$gateway_ip dev wlan20

echo "Client ok!"
exit 0
