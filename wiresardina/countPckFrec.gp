set terminal png s size 800,600
set output 'pckCountFrec.png' 
#set boxwidth 0.9 absolute
set style fill  solid 0.05 border -1
set style histogram clustered gap 2 title  offset character 0, 0, 0
set datafile missing '-'
set style data histograms
set xtics border in scale 1,0.5 nomirror  offset character 0,50,1000
set xtics  norangelimit
#set yrange [0: 100] noreverse nowriteback
set ylabel "Frecuency "
#set xlabel "Packet length, bytes"	
set xlabel "  0                                                     500                                                      1000                                                      1500"           
set title "Frecuency vs length size" 
plot 'pcktCountFrec.txt' using 2:xtic(3) ti col
pause -1
