#!/bin/bash

seedList="129 137 279 672 195 666 429 762 821 483"

for i in $seedList; do
    ./grasp instances_WEN/transformed_Data30_a.txt 3 300 50 3000 100 0.5 1 $i >> "Final results"/wen30a
done
echo "Termine 30a"
for i in $seedList; do
    ./grasp instances_WEN/transformed_Data30_b.txt 3 300 50 3000 100 0.5 1 $i >> "Final results"/wen30b
done
echo "Termine 30b"
for i in $seedList; do
    ./grasp instances_WEN/transformed_Data30_c.txt 3 300 50 3000 100 0.5 1 $i >> "Final results"/wen30c
done
echo "Termine 30c"
for i in $seedList; do
    ./grasp instances_WEN/transformed_Data30_d.txt 3 300 50 3000 100 0.5 1 $i >> "Final results"/wen30d
done
echo "Termine 30d"
