#!/bin/ash

/root/ap17.sh

echo 'openwrt17' >| /tmp/wsol/LPM_TO_SEND.txt
# 建立向上的路由
/root/tcpsrv_up-arm1900 &
/root/findrouter.sh
/root/chooserouter-arm1900 /tmp/wsol/routerlist.txt >| /tmp/wsol/bestrouter.txt
while [[ $? -ne 0 ]]; do
	echo "No matched AP"
	sleep 2
	/root/findrouter.sh
	/root/chooserouter-arm1900 /tmp/wsol/routerlist.txt >| /tmp/wsol/bestrouter.txt
done
sleep 1
/root/clientAP_17.sh
sleep 2
nexthop_num=`grep '^ssid' /tmp/wsol/bestrouter.txt | sed -r 's/ssid="openwrt(.*)"/\1/'`
/root/tcpcli-arm1900 192.168.$nexthop_num.$nexthop_num /tmp/wsol/LPM_TO_SEND.txt

# After getting data, tcpsrv_up-arm1900 call link_build_up.sh

# Wait for tcpsrv_up-arm1900 end.
ps | grep tcpsrv_up-arm1900 | grep -v grep
while [[ $? -eq 0 ]]; do
	sleep 1
	ps | grep tcpsrv_up-arm1900 | grep -v grep
done
# Bacup for bestrouter to client
cp /tmp/wsol/bestrouter.txt /root/bakup/bestrouter.bak
# Bacup for route table
echo "#!/bin/ash" >| /root/bakup/routebak.sh
route -n | tail -n +3 | awk '{print "route add -net "$1" gw "$2" netmask "$3" dev "$8}' >> /root/bakup/routebak.sh
