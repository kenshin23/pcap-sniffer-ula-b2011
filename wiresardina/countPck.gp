set terminal png s size 420,320
set output 'pckCount.png' 
#set boxwidth 0.9 absolute
set style fill  solid 1.00 border -1
set style histogram clustered gap 2 title  offset character 0, 0, 0
set datafile missing '-'
set style data histograms
set xtics border in scale 1,0.5 nomirror rotate by -45  offset character 0, 0, 0
set xtics  norangelimit
#set yrange [0: 100] noreverse nowriteback
set ylabel "% "
set xlabel "Packet length, bytes"	
#set xrange [0:]
set title "Packet length distribution(Most frequent length)" 
plot 'packtCount.txt' using 2:xtic(1) ti col
pause -1
