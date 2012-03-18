set terminal png
set output 'ImgIpFrecKb.png'
set boxwidth 0.5 absolute
set nokey
#set log y
set style fill   solid 1.00 border lt -1
#set key inside right top vertical Right noreverse noenhanced autotitles nobox
#set style histogram clustered gap 1 title  offset character 0, 0, 0
set datafile missing '-'
set style data histograms
#set xtics border in scale 0,0 nomirror rotate by -45  offset character 0, 0, 0
set xtics  norangelimit font ",8"
#set xtics   ()
set title "Histograma de Volumen de data" 
##PARES DE IP:
set ylabel "Frecuencia"
##KB
set xlabel "Volumen de data, KB"
#set xrange [ 1.00000 : 300000. ] noreverse nowriteback
#i = 22
plot 'graphdata_ipfrec_kb.dat' with boxes
