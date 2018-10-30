kill `pgrep wpa_supplicant`
kill `pgrep hostapd`
kill `pgrep tcpsrv_up`
kill `pgrep tcpsrv_down`
kill `pgrep tcpcli`
kill `ps | grep WSOL_Relay.sh | grep -v grep | awk '{print $1}'`
kill `pgrep IMLM_RU`
kill `pgrep chooserouter-arm1900`
kill `pgrep findrouter.sh`
kill `pgrep ap15.sh`
kill `pgrep clientAP`
