#!/bin/bash

seedList="129 137 279 672 195 666 429 762 821 483 999 729 318 349 287 241 476 555 581 573"

for i in $seedList; do
    ./grasp ../instances_WEN/transformed_Data50_a.txt 3 300 50 3000 50 0.5 1 $i >> ../Final-results/wen50a.txt
done
echo "Termine 50a"
for i in $seedList; do
    ./grasp ../instances_WEN/transformed_Data50_b.txt 3 300 50 3000 50 0.5 1 $i >> ../Final-results/wen50b.txt
done
echo "Termine 50b"
for i in $seedList; do
    ./grasp ../instances_WEN/transformed_Data50_c.txt 3 300 50 3000 50 0.5 1 $i >> ../Final-results/wen50c.txt
done
echo "Termine 50c"
for i in $seedList; do
    ./grasp ../instances_WEN/transformed_Data50_d.txt 3 300 50 3000 50 0.5 1 $i >> ../Final-results/wen50d.txt
done
echo "Termine 50d"
for i in $seedList; do
    ./grasp ../instances_WEN/transformed_Data50_e.txt 3 300 50 3000 50 0.5 1 $i >> ../Final-results/wen50e.txt
done
echo "Termine 50e"
