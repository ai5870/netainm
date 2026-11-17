set terminal pdf
set output "Throughput.pdf"
set title "Throughput Vs Packet Size"
set xlabel "node numbers"
set ylabel "Throughput in bps"
plot "throughput.txt" using 1:2 with linespoints title "1024", "throughput.txt" using 1:3 with linespoints title "512"
