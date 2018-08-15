#!/bin/bash

seedList="279 672 195 666 429"

for i in $seedList; do
    ./grasp instances_MI/R1_4_1_200m.txt 3 3000 20 5000 200 0.5 1 $i >> "Final results"/mi4_1_200
done
echo "Termine 4_1"
for i in $seedList; do
    ./grasp instances_MI/R1_4_2_200m.txt 3 3000 20 5000 200 0.5 1 $i >> "Final results"/mi4_2_200
done
echo "Termine 4_2"
for i in $seedList; do
    ./grasp instances_MI/R1_4_3_200m.txt 3 3000 20 5000 200 0.5 1 $i >> "Final results"/mi4_3_200
done
echo "Termine 4_3"
for i in $seedList; do
    ./grasp instances_MI/R1_4_4_200m.txt 3 3000 20 5000 200 0.5 1 $i >> "Final results"/mi4_4_200
done
echo "Termine 4_4"