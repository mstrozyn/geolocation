#!/bin/bash

rm -f test.txt
for i in $(seq 1 $1); do taskset --cpu-list 1 ./geolocation_test.py --executable ../out/locate --database database.csv | grep Final >> test.txt; done;

sort -k6 -n test.txt | head -n 1
