#!/bin/ash
# Use iw dev XXX station del to del connecting point
# and link route table
# Argument: 192.168.XXX.0 ($1)

gw_ip=`route -n | grep $1 | awk '{print $2}'`	# if gw=192.168.15.0
echo "Ready to del $1"
# IMLM_S.c will call this shell for all router whitch discontinue its heartbeats.
# But this shell only need to execute once. So this 'if' can ignore these undesired calling.
if [[ -z $gw_ip ]]; then
	echo "$1 has already been deleted."
	exit 0
fi
gw_num=`echo $gw_ip | awk -F. '{print $4}'`		# gw_num=15
gw_ssid=openwrt$gw_num							# gw_ssid=openwrt15
gw_inMAC=`grep $gw_ssid /tmp/wsol/routerlist.txt -B 2 | head -n 1 | awk -F: -v OFS=: '{$1=20; print $0}'`	# get mac from routerlist.txt

iw dev wlan10 station del $gw_inMAC 			# del connecting point

gw_relate_ip_num=`route -n | grep $gw_ip | wc -l`
for i in $(seq 1 $gw_relate_ip_num); do
	gw_relate_ip=`route -n | grep $gw_ip | awk '{print $1}' | tail -n $i | head -n 1`
	route del -net $gw_relate_ip netmask 255.255.255.0
done

exit 0
