#!/bin/ash

sleep 1

echo "KILL ALL PROCESSES!"
/root/killthemall.sh
echo "REWIND ROUTE TABLE!"
/etc/init.d/network restart
sleep 2
echo "RESTART WSOL PROCESSES!"
source /etc/profile
/root/WSOL_Relay.sh > /tmp/WSOL_Relay.out 2>&1 &
