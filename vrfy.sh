while true; do
#clear && ps aux | grep crisis | echo " $(($(wc -l)-2)) hijos reales"
clear && ps aux | grep crisis
sleep 1
done
