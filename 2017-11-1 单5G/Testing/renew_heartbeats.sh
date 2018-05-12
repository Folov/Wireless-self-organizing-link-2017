#!/bin/ash

while [ 1 ]; do
	sleep 6
	point_num=`env | grep ^openwrt | sed 's/=.//' | wc -l`
	for i in $(seq 1 $point_num); do
		point_name=`env | grep ^openwrt | sed 's/=1//' | tail -n $i | head -n 1`
		export $point_name=0
	done
done
