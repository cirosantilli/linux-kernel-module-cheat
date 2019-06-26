#!/usr/bin/env gnuplot

set terminal png noenhanced size 800, 1400
set output input_noext . ".tmp.png"
set multiplot layout 5,1 title "\nC++ Heap vs BST vs Hash map insert time" font ",22"
set xlabel "container size"
set ylabel "insert time (ns)"
set title font ",16"

set title "Heap (std::priority_queue)"
plot input_noext . ".dat" using 1:2 notitle

set title "Heap (zoom)"
set yrange [0:heap_zoom_max]
plot input_noext . ".dat" using 1:2 notitle

set title "BST (std::set)"
set yrange [*:*]
plot input_noext . ".dat" using 1:3 notitle

set title "Hash map (std::unordered_set)"
set yrange [*:*]
plot input_noext . ".dat" using 1:4 notitle

set title "Hash map zoom"
set yrange [0:hashmap_zoom_max]
plot input_noext . ".dat" using 1:4 notitle
