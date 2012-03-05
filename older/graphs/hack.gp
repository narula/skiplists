#!/usr/bin/gnuplot

set out "hack-tmp.eps"
set terminal postscript eps dl 2
set size 2.25, 1.875
set style data linespoints
set yrange [0:]
set xlabel "size"
set ylabel "time"

f2(x) = a2*log(x) + b2
a2 = 10; b2 = 10
fit f2(x) 'hack.dat2' using 1:2 via a2, b2

f4(x) = a4*log(x) + b4
a4 = 10; b4 = 10
fit f4(x) 'hack.dat4' using 1:2 via a4, b4

plot 'hack.dat2' using 1:2 title "lookups log=2" with linespoints lt 1 lw 3,\
	 f2(x) title "Lookups Fit 2 Data", \
	 'hack.dat4' using 1:2 title "lookups log=4" with linespoints lt 2 lw 3,\
	 f4(x) title "Lookups Fit 4 Data"
system "epstopdf hack-tmp.eps"
system "ps2pdf14 -dPDFSETTINGS=/prepress hack-tmp.pdf hack.pdf"
system "rm hack-tmp.*"
