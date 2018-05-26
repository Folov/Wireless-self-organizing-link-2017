#!/bin/ash
# Use iw dev XXX station del to del connecting point
# and link route table
# Argument: 192.168.XXX.0 ($1)

gw_ip=`route -n | grep $1 | awk '{print $2}'`	# if gw=192.168.11.15
echo "Ready to del $1"
# IMLM_S.c will call this shell for all router whitch discontinue its heartbeats.
# But this shell only need to execute once. So this 'if' can ignore these undesired calling.
if [[ -z $gw_ip ]]; then
	echo "$1 has already been deleted."
	exit 0
fi
gw_num=`echo $gw_ip | awk -F. '{print $4}'`		# gw_num=15
gw_ssid=openwrt$gw_num							# gw_ssid=openwrt15

gw_num_16=`printf %x $gw_num`
if [ $gw_num -le 15 ]; then
	gw_num_16=0$gw_num_16
fi
gw_inMAC=`iw dev wlan10 station dump | grep "^Station 20:$gw_num_16:" | awk '{print $2}'`
if [[ -z $gw_inMAC ]]; then
	echo "$gw_inMAC NOT in iw dump list! Try 21:XX:XX:XX:XX:XX"
	gw_inMAC=`iw dev wlan10 station dump | grep "^Station 21:$gw_num_16:" | awk '{print $2}'`
fi
if [[ -z $gw_inMAC ]]; then
	echo "$gw_inMAC NOT in iw dump list! Exit link_del.sh!"
	exit 0
fi

iw dev wlan10 station del $gw_inMAC 			# del connecting point

gw_relate_ip_num=`route -n | grep $gw_ip | wc -l`
for i in $(seq 1 $gw_relate_ip_num); do
	gw_relate_ip=`route -n | grep $gw_ip | awk '{print $1}' | tail -n $i | head -n 1`
	route del -net $gw_relate_ip netmask 255.255.255.0
done

exit 0
