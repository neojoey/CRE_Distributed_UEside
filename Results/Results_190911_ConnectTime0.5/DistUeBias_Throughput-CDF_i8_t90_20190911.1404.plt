set terminal png
set output "DistUeBias_Throughput-CDF_i8_t90_20190911.1404.png"
set title "Throughput CDF"
set xlabel "Throughput (Kbps)"
set ylabel "CDF"

set xrange [0: 692.373]
plot "-"  title "Throughput Data" with linespoints
0.402235 0.02
3.9419 0.04
5.17542 0.06
10.3777 0.08
26.1721 0.1
31.3475 0.12
40.1698 0.14
43.6559 0.16
51.3788 0.18
69.8279 0.2
69.962 0.22
74.6279 0.24
84.6034 0.26
88.6793 0.28
92.943 0.3
105.01 0.32
111.714 0.34
133.435 0.36
144.831 0.38
198.114 0.4
199.857 0.42
212.889 0.44
213.533 0.46
223.24 0.48
239.893 0.5
267.54 0.52
281.511 0.54
287.196 0.56
287.705 0.58
288.697 0.6
324.603 0.62
331.817 0.64
336.429 0.66
347.182 0.68
354.905 0.7
366.382 0.72
375.848 0.74
377.457 0.76
378.53 0.78
381.533 0.8
383.249 0.82
399.687 0.84
418.78 0.86
438.221 0.88
441.171 0.9
443.96 0.92
445.166 0.94
455.061 0.96
456.59 0.98
662.373 1
e
