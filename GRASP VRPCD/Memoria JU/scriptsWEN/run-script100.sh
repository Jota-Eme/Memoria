#!/bin/bash

seedList="129 137 279 672 195 666 429 762 821 483 999 729 318 349 287 241 476 555 581 573"

for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data100_a.txt 10 300 50 3000 30 0.5 1 $i >> ../Final-results/wen100a.txt
done
echo "Termine 100a"
for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data100_b.txt 10 300 50 3000 30 0.5 1 $i >> ../Final-results/wen100b.txt
done
echo "Termine 100b"
for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data100_c.txt 10 300 50 3000 30 0.5 1 $i >> ../Final-results/wen100c.txt
done
echo "Termine 100c"
for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data100_d.txt 10 300 50 3000 30 0.5 1 $i >> ../Final-results/wen100d.txt
done
echo "Termine 100d"
for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data100_e.txt 10 300 50 3000 30 0.5 1 $i >> ../Final-results/wen100e.txt
done
echo "Termine 100e"