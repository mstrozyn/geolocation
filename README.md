## geolocation lookup application
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
./prepare ../geolocation/database.csv
```
## Run the program
```
cd out
./locate ../geolocation/database.csv
```
## Run the tests
```
cd geolocation
./geolocation_test.py --executable ../out/locate --database ../geolocation/database.csv
```
## Development and test environment
Developed and tested on ubuntu 20.04.
