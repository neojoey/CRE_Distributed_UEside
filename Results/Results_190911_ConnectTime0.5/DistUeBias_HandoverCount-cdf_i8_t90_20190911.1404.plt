set terminal png
set output "DistUeBias_HandoverCount-cdf_i8_t90_20190911.1404.png"
set title "Handover Frequency CDF"
set xlabel "Handover Frequency"
set ylabel "CDF"

set xrange [0: 15 ]
plot "-"  title "Handover Count Data" with linespoints
0 0.02
0 0.04
0 0.06
0 0.08
0 0.1
0 0.12
0 0.14
0 0.16
0 0.18
0 0.2
0 0.22
0 0.24
0 0.26
0 0.28
0 0.3
0 0.32
0 0.34
0 0.36
0 0.38
0 0.4
0 0.42
0 0.44
0 0.46
0 0.48
0 0.5
0 0.52
0 0.54
0 0.56
0 0.58
0 0.6
0 0.62
0 0.64
0 0.66
0 0.68
1 0.7
1 0.72
1 0.74
1 0.76
1 0.78
1 0.8
1 0.82
1 0.84
6 0.86
9 0.88
11 0.9
11 0.92
11 0.94
13 0.96
13 0.98
15 1
e
