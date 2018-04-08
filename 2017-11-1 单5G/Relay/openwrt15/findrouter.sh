#!/bin/ash
# This file is used to find all APs arround router, if iw dev xxx scan go wrong,
# it will return 1 and into the loop, rescan AP until it works well.
echo "find router"
while [ 1 ]; do
	if iw dev wlan20 scan >| /tmp/wsol/iwscan.txt
	then
		egrep -w '^	SSID|^	signal|^BSS' /tmp/wsol/iwscan.txt | sed 's/(on wlan20)//; s/	//; s/BSS //; s/signal: //; s/SSID: //; s/ dBm//; s/ -- associated//; s/^-//' >| /tmp/wsol/routerlist.txt
		# Replace blank line to 'blank_ssid', lest wrong read by chooserouter.
		sed -i 's/^$/blank_ssid/' /tmp/wsol/routerlist.txt
		echo "findrouter ok!"
		exit 0
	fi
	ifconfig wlan20 up
	sleep 1
done
