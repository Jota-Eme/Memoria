#!/bin/bash

seedList="129 137 279 672 195 666 429 762 821 483 999 729 318 349 287 241 476 555 581 573"

for i in $seedList; do
    .././grasp ../instances_MI/R1_4_1_200m.txt 3 3000 20 6000 100 0.5 1 $i >> ../Final-results/mi4_1_200.txt
done
echo "Termine 4_1_200"
for i in $seedList; do
    .././grasp ../instances_MI/R1_4_2_200m.txt 3 3000 20 6000 100 0.5 1 $i >> ../Final-results/mi4_2_200.txt
done
echo "Termine 4_2_200"
for i in $seedList; do
    .././grasp ../instances_MI/R1_4_3_200m.txt 3 3000 20 6000 100 0.5 1 $i >> ../Final-results/mi4_3_200.txt
done
echo "Termine 4_3_200"
for i in $seedList; do
    .././grasp ../instances_MI/R1_4_4_200m.txt 3 3000 20 6000 100 0.5 1 $i >> ../Final-results/mi4_4_200.txt
done
echo "Termine 4_4_200"
for i in $seedList; do
    .././grasp ../instances_MI/R1_4_5_200m.txt 3 3000 20 6000 100 0.5 1 $i >> ../Final-results/mi4_5_200.txt
done
echo "Termine 4_5_200"