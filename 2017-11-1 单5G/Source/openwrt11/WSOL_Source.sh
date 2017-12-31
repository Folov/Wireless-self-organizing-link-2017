#!/bin/ash

./ap11.sh

# 建立向下的路由（阻塞于tcpsrv_down-arm1900）
./tcpsrv_down-arm3200
# Sometimes occur "bind err: address in use"
while [[ $? -ne 0 ]]; do
	kill `ps | grep tcpsrv_down-arm1900 | grep -v grep | awk '{print $1}'`
	sleep 1
	./tcpsrv_down-arm3200
done

./link_build_down.sh

# Send LPM to subnet.
down_num=`tail -n 1 /tmp/wsol/LPM_GET.txt | head -n 1 | sed 's/openwrt//'`
./tcpcli-arm3200 192.168.$down_num.$down_num /tmp/wsol/LPM_TO_SEND.txt

# Bacup for route table
echo "#!/bin/ash" >| ./bakup/routebak.sh
route -n | tail -n +3 | awk '{print "route add -net "$1" gw "$2" netmask "$3" dev "$8}' >> ./bakup/routebak.sh
