#!/bin/ash
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
