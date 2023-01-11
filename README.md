## geolocation lookup
## Build the "prepare" and "locate" command line tools:
Make build directory:
```
mkdir out
```
Generate the makefile:
```
cd out
cmake ../geolocation
```
Compile:
```
make
```
## Prepare the database
Unpack and preprocess the database into binary format:
```
cd geolocation
unzip database.zip
cd ../out
./prepare ../geolocation/database.csv
```
The "prepare" command preprocess the csv file in order to create database in binary format. The resulting database contains only needed records i.e. ip range start (csv column1), country code (csv column3) and the city (csv column6)). Preprocessing is done using https://github.com/ben-strasser/fast-cpp-csv-parser header only library.

The format of the binary database is as follows:
```
+--------------------------------------------------------+
|                                                        |
|             4 bytes (DB_COUNT_RECORD_SIZE)             | db size
|                                                        |
+--------------------------------------------------------+
|                                                        |
|   (DB_COUNT_RECORD_SIZE * DB_INDEX_RECORD_SIZE) bytes  | db index (ip range start)
|                                                        |
+--------------------------------------------------------+
|                                                        |
| (DB_COUNT_RECORD_SIZE * DB_LOCATION_RECORD_SIZE) bytes | db records (country code and city)
|                                                        |
+--------------------------------------------------------+
```
## Run the program:
```
cd out
./locate ../geolocation/database.csv
```
The "locate" program maps the binary database file into process address space and all the search is done using pointer arithmetic. There is no separate data structures.
## Run the tests on the specific cpu to improve the cache hit:
```
cd geolocation
taskset --cpu-list 1 ./geolocation_test.py --executable ../out/locate --database ../geolocation/database.csv
Database loaded Memory usage: 3.13mb Load time: 40μs
OK    1.0.0.0 US Los Angeles Memory usage: 3.13mb Lookup time: 42μs
OK    71.6.28.0 US San Jose Memory usage: 3.13mb Lookup time: 25μs
OK    71.6.28.255 US San Jose Memory usage: 3.13mb Lookup time: 13μs
OK    71.6.29.0 US Concord Memory usage: 3.13mb Lookup time: 11μs
OK    53.103.144.0 DE Stuttgart Memory usage: 3.13mb Lookup time: 24μs
OK    53.255.255.255 DE Stuttgart Memory usage: 3.13mb Lookup time: 10μs
OK    54.0.0.0 US Rahway Memory usage: 3.13mb Lookup time: 12μs
OK    223.255.255.255 AU Brisbane Memory usage: 3.13mb Lookup time: 32μs
OK    5.44.16.0 GB Hastings Memory usage: 3.13mb Lookup time: 18μs
OK    8.24.99.0 US Hastings Memory usage: 3.13mb Lookup time: 15μs
Final points for 10 measurements:  52.237044000000004
```
In order to achieve best perfomance the "taskset" command is used to schedule execution on the specified cpu maximising the cache hit.
## Best score from 100 subsequent test runs:
## Development and test environment:
Developed and tested on ubuntu 20.04, 64 bit, Intel Core i3.
