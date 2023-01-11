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
Preprocess the database into binary format:
```
cd out
./prepare ../geolocation/database.csv
```
The "prepare" command preprocess the csv file in order to create database in binary format. The resulting database contains only needed records i.e. ip range start (csv column1), country code (csv column3) and the city (csv column6)). Preprocessing is done using https://github.com/ben-strasser/fast-cpp-csv-parser header only library.

The format of the binary database is as follows:
+--------------------------------+-----------------------------------------------------+--------------------------------------------------------+
| 4 bytes (DB_COUNT_RECORD_SIZE) | (DB_COUNT_RECORD_SIZE * DB_INDEX_RECORD_SIZE) bytes | (DB_COUNT_RECORD_SIZE * DB_LOCATION_RECORD_SIZE) bytes |
+--------------------------------+-----------------------------------------------------+--------------------------------------------------------+
             db size                             db index (ip range start)                        db record for the corresponding index
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
```
In order to achieve best perfomance the "taskset" command is used to schedule execution on the specified cpu maximising the cache hit.
## Best score from 100 subsequent test runs:
## Development and test environment:
Developed and tested on ubuntu 20.04, 64 bit, Intel Core i3.
