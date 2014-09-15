while [ 1 == 1 ]
do
echo "{\"kcm-cmd\":\"echo\",\"message\":\"this is a test\"}" | nc -w 2 localhost 9100
done
