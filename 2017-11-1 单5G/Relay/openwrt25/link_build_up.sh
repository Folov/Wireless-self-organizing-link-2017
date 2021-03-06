#!/bin/ash

echo "Establish static route."
locate_number=`grep -n "$SSID" /tmp/wsol/LPM_TOP.txt | sed 's/:.*//'`
lpm_number=`wc -l /tmp/wsol/LPM_TOP.txt | sed 's/ .*$//'`
gw_num=`sed -n $(expr $locate_number + 1)p /tmp/wsol/LPM_TOP.txt | sed 's/openwrt//'`
gw_ip=192.168.$gw_num.$gw_num

for i in $(seq $(expr $locate_number + 1) $lpm_number); do
	to_num=`head -n $i /tmp/wsol/LPM_TOP.txt | tail -n 1 | sed 's/openwrt//'`
	echo "routing to 192.168.$to_num.0 ..."
	route add -net 192.168.$to_num.0 netmask 255.255.255.0 gw $gw_ip dev wlan20
	echo "link_build_up ok"
done
# Send LPM to subnet.
down_num=`tail -n 1 /tmp/wsol/LPM_GET.txt | head -n 1 | sed 's/openwrt//'`
/root/tcpcli-arm1900 192.168.$down_num.$down_num /tmp/wsol/LPM_TOP.txt
# Sometimes occur "connect error: Host is unreachable"
while [[ $? -ne 0 ]]; do
	echo "Wait for tcpcli to subnet..."
	sleep 1
	/root/tcpcli-arm1900 192.168.$down_num.$down_num /tmp/wsol/LPM_TOP.txt
done

exit 0
