while true; do
clear && ps aux | grep crisis | echo " $(($(wc -l)-2)) hijos reales"
sleep 1
done
