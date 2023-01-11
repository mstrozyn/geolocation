#!/bin/bash

for i in $(seq 1 $1); do taskset --cpu-list 1 ./geolocation_test.py --executable ../out/locate --database database.csv | grep Final >> results; done;

sort -k6 -n results > results_sorted
echo min: `head -n 1 results_sorted | awk '{print $6}'`
echo max: `tail -n 1 results_sorted | awk '{print $6}'`
echo avg: `awk '{ total += $6 } END { print total/NR }' results_sorted`

rm -f results
rm -f results_sorted
