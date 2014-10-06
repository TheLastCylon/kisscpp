echo '{"kcm-cmd":"get_statistics","type":"full"}'     | nc -w 2 localhost 9100
echo '{"kcm-cmd":"get_statistics","type":"gathered"}' | nc -w 2 localhost 9100
echo '{"kcm-cmd":"get_statistics","type":"current"}'  | nc -w 2 localhost 9100

echo '{"kcm-cmd":"get_statistics","type":"full"}'     | nc -w 2 localhost 9200
echo '{"kcm-cmd":"get_statistics","type":"gathered"}' | nc -w 2 localhost 9200
echo '{"kcm-cmd":"get_statistics","type":"current"}'  | nc -w 2 localhost 9200

echo '{"kcm-cmd":"get_statistics","type":"full"}'     | nc -w 2 localhost 9300
echo '{"kcm-cmd":"get_statistics","type":"gathered"}' | nc -w 2 localhost 9300
echo '{"kcm-cmd":"get_statistics","type":"current"}'  | nc -w 2 localhost 9300
