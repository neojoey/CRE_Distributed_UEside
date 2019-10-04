set terminal png
set output "DistUeBias_Throughput-CDF_i8_t90_20190909.2243.png"
set title "Throughput CDF"
set xlabel "Throughput (Kbps)"
set ylabel "CDF"

set xrange [0: 692.32]
plot "-"  title "Throughput Data" with linespoints
0.402235 0.02
3.9419 0.04
5.20223 0.06
11.8525 0.08
26.1721 0.1
31.3475 0.12
40.1698 0.14
43.6559 0.16
51.3788 0.18
69.8279 0.2
69.962 0.22
84.6034 0.24
84.8179 0.26
90.476 0.28
92.943 0.3
105.01 0.32
111.714 0.34
133.435 0.36
144.831 0.38
198.114 0.4
199.857 0.42
212.889 0.44
223.24 0.46
239.893 0.48
267.54 0.5
281.537 0.52
287.196 0.54
287.705 0.56
288.697 0.58
309.989 0.6
331.79 0.62
336.456 0.64
347.182 0.66
354.905 0.68
357.989 0.7
375.848 0.72
377.457 0.74
378.53 0.76
381.533 0.78
383.249 0.8
399.687 0.82
409.743 0.84
418.807 0.86
438.221 0.88
441.171 0.9
443.96 0.92
445.166 0.94
455.061 0.96
456.59 0.98
662.32 1
e
