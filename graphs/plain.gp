#!/usr/bin/gnuplot

set out "plain-tmp.eps"
set terminal postscript eps dl 2
set size 2.25, 1.875
set style data linespoints
set yrange [0:]
set xlabel "size"
set ylabel "time"

f2(x) = a2*log(x) + b2
a2 = 114; b2 = -670
fit f2(x) 'plain.dat2' using 1:3 via a2, b2

f4(x) = a4*log(x) + b4
a4 = 164; b4 = -1118
fit f4(x) 'plain.dat4' using 1:3 via a4, b4

f8(x) = a8*log(x) + b8
a8 = 252; b8 = -1894
fit f8(x) 'plain.dat8' using 1:3 via a8, b8

plot 'plain.dat2' using 1:2 title "inserts fanout=2" with linespoints lt 1 lw 3,\
	 'plain.dat2' using 1:3 title "lookups fanout=2" with linespoints lt 2 lw 3,\
	 f2(x) title "Lookups Fit 2 Data", \
	 'plain.dat4' using 1:2 title "inserts fanout=4" with linespoints lt 1 lw 3,\
	 'plain.dat4' using 1:3 title "lookups fanout=4" with linespoints lt 2 lw 3,\
	 f4(x) title "Lookups Fit 4 Data", \
	 'plain.dat8' using 1:2 title "inserts fanout=8" with linespoints lt 1 lw 3,\
	 'plain.dat8' using 1:3 title "lookups fanout=8" with linespoints lt 2 lw 3,\
	 f8(x) title "Lookups Fit 8 Data"
system "epstopdf plain-tmp.eps"
system "ps2pdf14 -dPDFSETTINGS=/prepress plain-tmp.pdf plain.pdf"
system "rm plain-tmp.*"
