#!/bin/bash

seedList="129 137 279 672 195 666 429 762 821 483 999 729 318 349 287 241 476 555 581 573"

for i in $seedList; do
    .././grasp ../instances_MI/R1_10_1_500m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi10_1_500.txt
done
echo "Termine 10_1_500"
for i in $seedList; do
    .././grasp ../instances_MI/R1_10_2_500m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi10_2_500.txt
done
echo "Termine 10_2_500"
for i in $seedList; do
    .././grasp ../instances_MI/R1_10_3_500m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi10_3_500.txt
done
echo "Termine 10_3_500"
for i in $seedList; do
    .././grasp ../instances_MI/R1_10_4_500m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi10_4_500.txt
done
echo "Termine 10_4_500"
for i in $seedList; do
    .././grasp ../instances_MI/R1_10_5_500m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi10_5_500.txt
done
echo "Termine 10_5_500"