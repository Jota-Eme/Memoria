#!/bin/bash

seedList="129 137 279 672 195 666 429 762 821 483 999 729 318 349 287 241 476 555 581 573"

for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data200_a.txt 10 300 50 2500 30 0.5 1 $i >> ../Final-results/wen200a.txt
done
echo "Termine 200a"
for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data200_b.txt 10 300 50 2500 30 0.5 1 $i >> ../Final-results/wen200b.txt
done
echo "Termine 200b"
for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data200_c.txt 10 300 50 2500 30 0.5 1 $i >> ../Final-results/wen200c.txt
done
echo "Termine 200c"
for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data200_d.txt 10 300 50 2500 30 0.5 1 $i >> ../Final-results/wen200d.txt
done
echo "Termine 200d"
for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data200_e.txt 10 300 50 2500 30 0.5 1 $i >> ../Final-results/wen200e.txt
done
echo "Termine 200e"