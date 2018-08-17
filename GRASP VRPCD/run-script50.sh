#!/bin/bash

seedList="129 137 279 672 195 666 429 762 821 483"

for i in $seedList; do
    ./grasp instances_WEN/transformed_Data50_a.txt 3 300 50 3000 100 0.5 1 $i >> "Final results"/wen50a
done
echo "Termine 50a"
for i in $seedList; do
    ./grasp instances_WEN/transformed_Data50_b.txt 3 300 50 3000 100 0.5 1 $i >> "Final results"/wen50b
done
echo "Termine 50b"
for i in $seedList; do
    ./grasp instances_WEN/transformed_Data50_c.txt 3 300 50 3000 100 0.5 1 $i >> "Final results"/wen50c
done
echo "Termine 50c"
for i in $seedList; do
    ./grasp instances_WEN/transformed_Data50_d.txt 3 300 50 3000 100 0.5 1 $i >> "Final results"/wen50d
done
echo "Termine 50d"
