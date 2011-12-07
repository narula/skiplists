#!/usr/bin/gnuplot

set out "plain-tmp.eps"
set terminal postscript eps dl 2
set size 2.25, 1.875
set style data linespoints
set xlabel "size"
set ylabel "time"

f(x) = a2*log(x) + b2
a2 = 215; b2 = -2078
#fit f(x) 'plain.dat' using 1:3 via a2, b2

plot 'plain.dat' using 1:2 title "inserts" with linespoints lt 1, 'plain.dat' using 1:3 title "lookups" with linespoints lt 2, f(x) title "Lookups Fit"
system "epstopdf plain-tmp.eps"
system "ps2pdf14 -dPDFSETTINGS=/prepress plain-tmp.pdf plain.pdf"
system "rm plain-tmp.*"
