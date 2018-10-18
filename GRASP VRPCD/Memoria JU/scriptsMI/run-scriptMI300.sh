#!/bin/bash

seedList="129 137 279 672 195 666 429 762 821 483 999 729 318 349 287 241 476 555 581 573"

for i in $seedList; do
    .././grasp ../instances_MI/R1_6_1_300m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi6_1_300.txt
done
echo "Termine 6_1_300"
for i in $seedList; do
    .././grasp ../instances_MI/R1_6_2_300m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi6_2_300.txt
done
echo "Termine 6_2_300"
for i in $seedList; do
    .././grasp ../instances_MI/R1_6_3_300m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi6_3_300.txt
done
echo "Termine 6_3_300"
for i in $seedList; do
    .././grasp ../instances_MI/R1_6_4_300m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi6_4_300.txt
done
echo "Termine 6_4_300"
for i in $seedList; do
    .././grasp ../instances_MI/R1_6_5_300m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi6_5_300.txt
done
echo "Termine 6_5_300"