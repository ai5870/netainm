# Goodput plot script (Experiment 6)
set terminal pdf
set output "goodput.pdf"
set title "Goodput Vs Packet Size"
set xlabel "node numbers"
set ylabel "Goodput in bps"
plot "goodput.txt" using 1:2 with linespoints title "1024", "goodput.txt" using 1:3 with linespoints title "512"
