#!/bin/ash

route del default
# 确保LPM_GET.txt文件是已更新的
sleep 2
lpm_utctime=`ls /tmp/wsol/LPM_GET.txt --full-time | awk '{print $6" "$7}'`
lpmtime=`date -d "$lpm_utctime" +%s`
lpmtime=`expr $lpmtime + 60`
nowtime=`date +%s`
while [ $lpmtime -lt $nowtime ]; do
	sleep 1
	lpm_utctime=`ls /tmp/wsol/LPM_GET.txt --full-time | awk '{print $6" "$7}'`
	lpmtime=`date -d "$lpm_utctime" +%s`
done

echo "Create file LPM_TO_SEND.txt"
cat /tmp/wsol/LPM_GET.txt > /tmp/wsol/LPM_TO_SEND.txt
# Sometimes occur "cat: can't open '/tmp/wsol/LPM_GET.txt': No such file or directory"
while [[ $? -ne 0 ]]; do
	echo "LPM_GET.txt do not exit..."
	sleep 1
	cat /tmp/wsol/LPM_GET.txt > /tmp/wsol/LPM_TO_SEND.txt
done
echo "Create LPM_TO_SEND.txt Done!"
echo $SSID >> /tmp/wsol/LPM_TO_SEND.txt
echo "Create file wsol_link.txt"
cat /tmp/wsol/LPM_TO_SEND.txt | tr "\n" "-" | sed 's/.$/\n/' >> /tmp/wsol/wsol_link.txt

echo "Establish static route."
lpm_number=`wc -l /tmp/wsol/LPM_GET.txt | sed 's/ .*$//'`
gw_num=`tail -n 1 /tmp/wsol/LPM_GET.txt | sed 's/openwrt//'`
gw_ip=192.168.$SSID_num.$gw_num

for i in $(seq 1 $lpm_number); do
	to_num=`tail -n $i /tmp/wsol/LPM_GET.txt | head -n 1 | sed 's/openwrt//'`
	echo "routing to 192.168.$to_num.0 ..."
	route add -net 192.168.$to_num.0 netmask 255.255.255.0 gw $gw_ip dev br-lan
	echo "link_build_down ok"
done
