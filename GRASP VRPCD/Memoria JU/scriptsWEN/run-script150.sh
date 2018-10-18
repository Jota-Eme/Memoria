#!/bin/bash

seedList="129 137 279 672 195 666 429 762 821 483 999 729 318 349 287 241 476 555 581 573"

for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data150_a.txt 10 300 50 2500 30 0.5 1 $i >> ../Final-results/wen150a.txt
done
echo "Termine 150a"
for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data150_b.txt 10 300 50 2500 30 0.5 1 $i >> ../Final-results/wen150b.txt
done
echo "Termine 150b"
for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data150_c.txt 10 300 50 2500 30 0.5 1 $i >> ../Final-results/wen150c.txt
done
echo "Termine 150c"
for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data150_d.txt 10 300 50 2500 30 0.5 1 $i >> ../Final-results/wen150d.txt
done
echo "Termine 150d"
for i in $seedList; do
    .././grasp ../instances_WEN/transformed_Data150_e.txt 10 300 50 2500 30 0.5 1 $i >> ../Final-results/wen150e.txt
done
echo "Termine 150e"