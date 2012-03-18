#set terminal pngcairo transparent enhanced font "arial,10" size 500, 350 
set terminal png enhanced font "arial,10" size 800, 600 
set output 'ImgPortsKb.png'
set boxwidth 0.9 absolute
set style fill   solid 1.00 border lt -1
set nokey
#set key inside right top vertical Right noreverse noenhanced nobox
set style histogram clustered gap 1 title  offset character 0, 0, 0
set datafile missing '-'
set style data histograms
set xtics border in scale 0,0 nomirror rotate by -45  offset character 0, 0, 0
set xtics  norangelimit font ",8"
#set xtics   ()
#set ytics 0,10,50
set mytics 4
set title "Distribución de números de\npuertos a través de los flujos." 
set xlabel "Aplicación o servicio"
set ylabel "%"
#set yrange [ 0.00000 : 300000. ] noreverse nowriteback
plot "< sort -s -n -r -k 2 graphdata_ports_kb.dat" using 2:xticlabels(1)
#pause -1 'Presione una tecla para continuar ...'
