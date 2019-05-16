#!/usr/bin/env gnuplot
set terminal png size 1024, 2048
set output "bst-vs-heap.tmp.png"
set multiplot layout 5,1 title "Heap vs BST vs Hash map insert time"
set xlabel "size"
set ylabel "nanoseconds"

set title "Heap"
plot "bst_vs_heap.dat" using 1:2 notitle

set title "Heap (zoom)"
set yrange [0:25]
plot "bst_vs_heap.dat" using 1:2 notitle

set title "BST"
set yrange [*:*]
plot "bst_vs_heap.dat" using 1:3 notitle

#set title "Hash map"
#set yrange [*:*]
#plot "bst_vs_heap.dat" using 1:4 notitle
#
#set title "Hash map zoom"
#set yrange [0:350]
#plot "bst_vs_heap.dat" using 1:4 notitle
