set terminal pngcairo enhanced fontscale 1.0 size 4096, 1024
set output 'result/hash-comparison.png'

unset colorbox

set palette positive nops_allcF maxcolors 0 gamma 1.0 color model HSV
set palette defined ( 0 0 1 1, 0.1 1 1 1, 0.2 0 0.75 1, 0.3 1 0.75 1, 0.4 0 0.5 1, 0.5 1 0.5 1, 0.6 0 0.25 1, 0.7 1 0.25 1, 0.8 0 0 1, 1 1 0 1 )

set auto x
set format y "10^{%L}"

set ytics nomirror
set grid ytics xtics

#set yrange [0.1:1000000000]
set xrange [0:*]
#set logscale y

set title "Time taken in nanosecond per each iteration"
set ylabel "nanoseconds"
set xlabel "iterations"

t=0
relative(x)=x - (t == 0 ? (t = x) : t)

plot '< awk "{x=x+\$3; print \$1,x}" data/hash-bplus-insert.dat'  using (relative($1)):2 title "Bplus Tree"        smooth unique with lines lw 0.5 \
   , '< awk "{x=x+\$3; print \$1,x}" data/hash-brute-insert.dat'  using (relative($1)):2 title "Brute Hashtable"   smooth unique with lines lw 0.5 \
   , '< awk "{x=x+\$3; print \$1,x}" data/hash-glib-insert.dat'   using (relative($1)):2 title "Glib2.0 GHash"     smooth unique with lines lw 0.5 \
   , '< awk "{x=x+\$3; print \$1,x}" data/hash-sparse-insert.dat' using (relative($1)):2 title "Google Sparsehash" smooth unique with lines lw 0.5
