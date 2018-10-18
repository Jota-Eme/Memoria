#!/bin/bash

seedList="129 137 279 672 195 666 429 762 821 483 999 729 318 349 287 241 476 555 581 573"

for i in $seedList; do
    .././grasp ../instances_MI/R1_8_1_400m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi8_1_400.txt
done
echo "Termine 8_1_400"
for i in $seedList; do
    .././grasp ../instances_MI/R1_8_2_400m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi8_2_400.txt
done
echo "Termine 8_2_400"
for i in $seedList; do
    .././grasp ../instances_MI/R1_8_3_400m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi8_3_400.txt
done
echo "Termine 8_3_400"
for i in $seedList; do
    .././grasp ../instances_MI/R1_8_4_400m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi8_4_400.txt
done
echo "Termine 8_4_400"
for i in $seedList; do
    .././grasp ../instances_MI/R1_8_5_400m.txt 3 3000 20 8000 100 0.5 1 $i >> ../Final-results/mi8_5_400.txt
done
echo "Termine 8_5_400"