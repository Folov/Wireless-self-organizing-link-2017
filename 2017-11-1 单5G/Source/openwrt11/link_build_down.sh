#!/bin/ash

SSID=openwrt11
SSID_num=11

route del default
echo "Create file LPM_TO_SEND.txt"
cat /tmp/wsol/LPM_GET.txt > /tmp/wsol/LPM_TO_SEND.txt
echo $SSID >> /tmp/wsol/LPM_TO_SEND.txt

echo "Establish static route, send LPM to subordinate router."
lpm_number=`wc -l /tmp/wsol/LPM_GET.txt | sed 's/ .*$//'`
gw_num=`tail -n 1 /tmp/wsol/LPM_GET.txt | sed 's/openwrt//'`
gw_ip=192.168.$SSID_num.$gw_num

for i in $(seq 1 $lpm_number); do
	to_num=`tail -n $i /tmp/wsol/LPM_GET.txt | head -n 1 | sed 's/openwrt//'`
	echo "routing to 192.168.$to_num.0 ..."
	route add -net 192.168.$to_num.0 netmask 255.255.255.0 gw $gw_ip dev br-lan
	echo "link_build_down ok"
done
