#!/bin/ash
# handling LED configuration

while [ -n "$1" ]; do
	case "$1" in
		-a ) echo "ALL SUPPORT LED TYPE:"
			 echo "-s [LED_PORT]: sata|usb1"
			 echo "-t [parameter]: xxx(ms) xxx(ms)"
			 echo "-d [parameter]: on|off" ;;

		-s ) param_led="$2"
			 if [ $param_led = "sata" ]; then
			 	param_port=/sys/class/leds/shelby\:white\:sata
			 elif [ $param_led = "usb1" ]; then
			 	param_port=/sys/class/leds/pca963x\:shelby\:white\:usb2
			 else
			 	echo "BAD PARAMETER!"
			 	break
			 fi
			 shift ;;
			 
		-t ) param_t_on="$2"
			 param_t_off="$3"
			if [ "$param_t_on" -gt 0 ] && [ "$param_t_off" -gt 0 ]; then
			 	echo "timer" > $param_port/trigger
			 	echo "$param_t_on" >| $param_port/delay_on
			 	echo "$param_t_off" >| $param_port/delay_off 
			 	shift
			 	shift
			else
				echo "BAD PARAMETER!"
				break
			fi 
			;;

		-d ) param_d="$2"
			 if [ $param_d = "on" ]; then
			 	echo "default-on" > $param_port/trigger
			 elif [ $param_d = "off" ]; then
			 	echo "none" > $param_port/trigger
			 else
			 	echo "BAD PARAMETER!"
			 	break
			 fi 
			 shift ;;
			 
		-h ) echo "Usage: $0 -s [LED_PORT] -t/-d [parameter]"
			 echo "See whitch type supported: $0 -a"
			 echo "Example: $0 -s sata -t 500 500"
			 echo "Example: $0 -s usb1 -d on"
			 ;;

		*  ) echo "$1 is not an option";;

	esac
	shift
done
