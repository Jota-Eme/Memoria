#!/bin/bash

seedList="129 137 279 672 195 666 429 762 821 483"

for i in $seedList; do
    ./grasp instances_WEN/transformed_Data20_a.txt 3 300 50 3000 100 0.5 1 $i >> "Final results"/wen20a
done
echo "Termine 20a"
for i in $seedList; do
    ./grasp instances_WEN/transformed_Data20_b.txt 3 300 50 3000 100 0.5 1 $i >> "Final results"/wen20b
done
echo "Termine 20b"
for i in $seedList; do
    ./grasp instances_WEN/transformed_Data20_c.txt 3 300 50 3000 100 0.5 1 $i >> "Final results"/wen20c
done
echo "Termine 20c"
for i in $seedList; do
    ./grasp instances_WEN/transformed_Data20_d.txt 3 300 50 3000 100 0.5 1 $i >> "Final results"/wen20d
done
echo "Termine 20d"
