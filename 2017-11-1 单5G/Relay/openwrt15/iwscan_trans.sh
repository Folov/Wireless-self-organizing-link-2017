#!/bin/ash
# 该脚本处理iwscan.txt文件
# 使其生成iwscan_trans.txt文件，并通过IMLM_RU程序发回源端
# 用于响应源端scan指令

egrep -w '^BSS|^\tfreq|^\tsignal|^\tSSID' /tmp/wsol/iwscan.txt | sed 's/(on wlan20)//; s/\t//; s/BSS/BSS:/; s/ /\t/' >| /tmp/wsol/iwscan_trans.txt