#!/bin/ash

# 该脚本在WSOL主程序运行之前执行，修改路由器wps按钮功能为
# =>短按：运行WSOL脚本并重定向输出到udp端口 192.168.12.200:20000 长按10s:恢复原始设置并重新运行建链脚本
# 该脚本仅需执行一次
# 运行该脚本需要socat支持

socat -V
if [[ $? -ne 0 ]]; then
	echo "socat: not found~"
	exit 1
fi

mkdir -p /etc/hotplug.d/button
touch /etc/hotplug.d/button/buttons
echo "#!/bin/sh" >| /etc/hotplug.d/button/buttons
echo "logger the button was \$BUTTON and the action was \$ACTION" >> /etc/hotplug.d/button/buttons

touch /etc/hotplug.d/button/00-button
cat /root/00-button.bak >| /etc/hotplug.d/button/00-button
uci add system button
uci set system.@button[-1].button=wps
uci set system.@button[-1].action=pressed
uci set system.@button[-1].handler='/root/Pre_start.sh'
uci add system button
uci set system.@button[-1].button=wps
uci set system.@button[-1].action=released
uci set system.@button[-1].handler='/root/Pre_restart.sh'
uci set system.@button[-1].min=10
uci set system.@button[-1].max=18
uci commit system

# need reboot
