set view map;

set term x11;

set xlabel "X"
set ylabel "Y"
set cblabel "SINR (dB)"

plot "rem.out" using ($1):($2):(10*log10($4)) with image