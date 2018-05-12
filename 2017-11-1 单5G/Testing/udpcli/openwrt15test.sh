#!/bin/bash

for (( i = 0; i < 20; i++ )); do
	sleep 3
	./udpcli-pc.exe 127.0.0.1 openwrt15
done

