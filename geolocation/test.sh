#!/bin/bash

rm -f test.txt
for i in $(seq 1 $1); do taskset --cpu-list 1 ./geolocation_test.py --executable ../out/locate --database database.csv | grep Final | sort -k6 -n >> test.txt; done;

echo min: `head -n 1 test.txt | awk '{print $6}'`
echo max: `tail -n 1 test.txt | awk '{print $6}'`
echo avg: `awk '{ total += $6 } END { print total/NR }' test.txt`
