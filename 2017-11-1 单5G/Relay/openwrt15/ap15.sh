#!/bin/ash
echo "ap15 up and initialize"
# Use this apxx.sh, we can build an AP_5G, change lan ip to 192.168.xx.xx, close dhcp.

# For wlan10 and wlan20 bssid
MAC_ADDR=`cat /sys/class/net/br-lan/address`
MAC_ADDR_LAST=`echo ${MAC_ADDR#*:}`
MAC_ADDR_WLAN10=10:$MAC_ADDR_LAST
MAC_ADDR_WLAN20=20:$MAC_ADDR_LAST
MAC_ADDR_WLAN30=30:$MAC_ADDR_LAST
if [ $MAC_ADDR_WLAN10 = $MAC_ADDR ]; then
	MAC_ADDR_WLAN10=11:$MAC_ADDR_LAST
fi
if [ $MAC_ADDR_WLAN20 = $MAC_ADDR ]; then
	MAC_ADDR_WLAN10=21:$MAC_ADDR_LAST
fi
if [ $MAC_ADDR_WLAN30 = $MAC_ADDR ]; then
	MAC_ADDR_WLAN10=31:$MAC_ADDR_LAST
fi

mkdir -p /tmp/wsol

echo "Set radio0/1..."
## arm for Linksys use radio0 as 5GHz AP.
uci set wireless.$DEVICE_5G1.type=mac80211
uci set wireless.$DEVICE_5G1.hwmode=11a
uci set wireless.$DEVICE_5G1.htmode=VHT80
uci set wireless.$DEVICE_5G1.channel=149
uci set wireless.$DEVICE_5G1.txpower=8
uci set wireless.$DEVICE_5G1.country=CN
uci set wireless.$DEVICE_5G1.disabled=0
## open 2.4G AP
uci set wireless.radio1.disabled=0

echo "Delete all wifi-iface"
## must be [1],[0]
uci delete wireless.@wifi-iface[1]
uci delete wireless.@wifi-iface[0]
uci commit wireless

## delete network interface 'wan6' and add new interface
echo "Add new interface"
if [ $DEVICE = Linksys_AC3200 ]; then
	uci set network.wan.ifname=eth1.2
	uci set network.wan._orig_ifname=eth1.2
	uci set network.lan.ifname=eth0.1
elif [ $DEVICE = Linksys_AC1900 ]; then
	uci set network.wan.ifname=eth1.2
	uci set network.wan._orig_ifname=eth1.2
	uci set network.lan.ifname=eth0.1
fi
uci delete network.wan6
uci set network.wan._orig_bridge=false
uci set network.wan.proto=static
uci set network.wan.ipaddr=$WAN_IP
uci set network.wan.netmask=255.255.255.0
uci set network.lan.proto=static
uci set network.lan.ipaddr=$LAN_IP
uci set network.lan.netmask=255.255.255.0
# uci set network.wwan=interface
# uci set network.wwan._orig_bridge=false
# uci set network.wwan.proto=static
# uci set network.wwan.netmask=255.255.255.0
uci commit network

# firewall config
echo "Change firewall settings"
uci set firewall.@defaults[0].forward=ACCEPT
uci set firewall.@zone[0].forward=ACCEPT
uci set firewall.@zone[0].network="lan"
uci set firewall.@zone[1].forward=ACCEPT
uci set firewall.@zone[1].network="wan"
uci set firewall.@zone[1].input=ACCEPT
uci commit firewall

# dhcp close
echo "Shut down dhcp"
uci delete dhcp.lan.leasetime
uci delete dhcp.lan.limit
uci delete dhcp.lan.start
uci set dhcp.lan.ignore=1
uci set dhcp.lan.ra_management=1
uci commit dhcp

# Restart service
echo "Restart"
/etc/init.d/dnsmasq restart
/etc/init.d/network restart
sleep 1
/etc/init.d/firewall restart

# iw configuration: add phy interface
iw phy phy0 interface add wlan10 type managed addr $MAC_ADDR_WLAN10
echo "wlan10 on phy0 has been created!"
iw phy phy0 interface add wlan20 type managed addr $MAC_ADDR_WLAN20
echo "wlan20 on phy0 has been created!"
iw phy phy1 interface add wlan30 type managed addr $MAC_ADDR_WLAN30
echo "wlan30 on phy1 has been created!"
# hostapd start(a pid file will be created)
sleep 1
hostapd -B -P /tmp/wsol/hostapd-wlan10.pid /root/hostapd-wlan10.conf
echo "5GAP has been created!"
sleep 1
hostapd -B -P /tmp/wsol/hostapd-wlan30.pid /root/hostapd-wlan30.conf
echo "2.4GAP has been created!"
