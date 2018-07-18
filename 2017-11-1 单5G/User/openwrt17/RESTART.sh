#!/bin/ash

sleep 1

echo "KILL ALL PROCESSES!"
/root/killthemall.sh
echo "REWIND ROUTE TABLE!"
/etc/init.d/network restart
