#!/usr/bin/env gnuplot

# https://github.com/cirosantilli/linux-kernel-module-cheat#bst-vs-heap-vs-hashmap
#
# A stacked plot with a single xlabel as shown at:
#
# * https://tex.stackexchange.com/questions/346882/creating-an-equally-distributed-multiplot-with-gnuplottex
# * http://gnuplot.sourceforge.net/demo_canvas_5.2/layout.html
#
# would be even nicer, but it was hard to get right, and this
# is pretty good already.

set terminal png noenhanced size 800, 1400
set output input_noext . ".tmp.png"
set multiplot layout 5,1 title "\nC++ Heap vs BST vs Hash map insert time" font ",22"
set lmargin 12
set label "Insert time (ns)" at screen 0.05,0.5 center front rotate font ",16"
set title font ",16"
set format y "%5.0f"

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

set xlabel "Container size" font ",16"
set title "Hash map (zoom)"
set yrange [0:hashmap_zoom_max]
plot input_noext . ".dat" using 1:4 notitle
