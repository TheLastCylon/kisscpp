echo '{"kcm-cmd":"set_log_level","type":"'$1'","severity":"'$2'"}' | nc -w 2 localhost 9100
