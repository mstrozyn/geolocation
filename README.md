# cross-platform geolocation lookup application

## Build
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

## Run the program
```
cd out
./Sample_app ../geolocation/database.csv

```

## Run the tests
```
cd geolocation
./geolocation_test.py --executable ../out/Sample_app --database database.csv

```
