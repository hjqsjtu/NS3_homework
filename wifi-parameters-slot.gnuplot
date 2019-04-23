set terminal postscript portrait enhanced lw 2 "Helvetica" 14

set size 1.0, 0.66

#-------------------------------------------------------
set out "wifi-throughput.eps"
set title "Throughput/slot"
set xlabel "slot(us)"
set xrange [2:22]
set ylabel "Throughput(Mbit/s) --- 5 Times Average"
set yrange [00:60]
plot "wifi-result.data" using 1:2 with lines title "WiFi Throughput"


set out "wifi-packet-loss.eps"
set title "Packet Loss/slot"
set xlabel "slot(us)"
set xrange [2:22]
set ylabel "% Packet Loss --- 5 Times Average"
set yrange [0:110]
plot "wifi-result.data" using 1:3 with lines title "WiFi Packet Loss"

set out "wifi-delay.eps"
set title "Delay/slot"
set xlabel "slot(us)"
set xrange [2:22]
set ylabel "Delay(ms) --- 5 Times Average"
set yrange [0:200]
plot "wifi-result.data" using 1:4 with lines title "WiFi Delay"

set out "wifi-delay-jitter.eps"
set title "Delay-Jitter/slot"
set xlabel "slot(us)"
set xrange [2:22]
set ylabel "Delay-Jitter(ms) --- 5 Times Average"
set yrange [0:10]
plot "wifi-result.data" using 1:5 with lines title "WiFi Delay-Jitter"

