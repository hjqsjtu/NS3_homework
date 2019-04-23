set terminal postscript portrait enhanced lw 2 "Helvetica" 14

set size 1.0, 0.66

#-------------------------------------------------------
set out "wifi-throughput.eps"
set title "Throughput/distance"
set xlabel "distance(m)"
set xrange [20:200]
set ylabel "Throughput(Mbit/s) --- 5 Times Average"
set yrange [0:60]
plot "wifi-result1.data" using 1:2 with lines title "WiFi Throughput"


set out "wifi-packet-loss.eps"
set title "Packet Loss/distance"
set xlabel "distance(m)"
set xrange [20:200]
set ylabel "% Packet Loss --- 5 Times Average"
set yrange [0:110]
plot "wifi-result1.data" using 1:3 with lines title "WiFi Packet Loss"

set out "wifi-delay.eps"
set title "Delay/distance"
set xlabel "distance(m)"
set xrange [20:200]
set ylabel "Delay(ms) --- 5 Times Average"
set yrange [100:2800]
plot "wifi-result2.data" using 1:2 with lines title "WiFi Delay"

set out "wifi-delay-jitter.eps"
set title "Delay-Jitter/distance"
set xlabel "distance(m)"
set xrange [20:200]
set ylabel "Delay-Jitter(ms) --- 5 Times Average"
set yrange [0:15]
plot "wifi-result2.data" using 1:3 with lines title "WiFi Delay-Jitter"

