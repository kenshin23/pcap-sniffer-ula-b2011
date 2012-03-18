set terminal png
set output 'ImgIpFrecflw.png'
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
set title "Grafica de Numero de Flujos" 
##PARES DE IP:
set ylabel "Frecuencia"
##KB
set xlabel "Numero de Flujos"
#set xrange [ 1.00000 : 300000. ] noreverse nowriteback
#i = 22
set pointsize 1
plot 'graphdata_ipfrec_flw.dat' with points lt -1 pt 6
