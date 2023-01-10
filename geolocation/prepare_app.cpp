#include <iostream>
#include <fstream>

#include "csv.h"
#include "definitions.h"

int PrepareDatabase(std::string path) {
    std::string filePath(path + ".bin");
    std::fstream databaseFile(filePath, std::ios::out | std::ios::binary);
    if (!databaseFile) {
        std::cerr << "error: can't create database file:" << filePath << std::endl;
        return EXIT_FAILURE;
    }

    uint32_t rowCount = 0;
    // reserve space for db count record
    databaseFile.write((const char *)&rowCount, DB_COUNT_RECORD_SIZE);

    // create db index (1st csv column)
    io::CSVReader<8, io::trim_chars<' '>, io::double_quote_escape<',','\"'> > ipReader(path);
    std::string column1, column2, column3, column4, column5, column6, column7, column8;
    while (ipReader.read_row(column1, column2, column3, column4, column5, column6, column7, column8)) {
        uint32_t ip = std::stol(column1);
        databaseFile.write((const char *)&ip, DB_INDEX_RECORD_SIZE);
        rowCount++;
    }

    // create db records (3rd and 6th csv column)
    io::CSVReader<8, io::trim_chars<' '>, io::double_quote_escape<',','\"'> > recordReader(path);
    while (recordReader.read_row(column1, column2, column3, column4, column5, column6, column7, column8)) {
        std::string location = column3 + std::string(",") + column6;
        databaseFile.write(location.c_str(), DB_LOCATION_RECORD_SIZE);
    }

    databaseFile.seekg(0);
    // write db count record
    databaseFile.write((const char *)&rowCount, DB_COUNT_RECORD_SIZE);
    databaseFile.close();

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "error: usage './prepare <database.csv>'" << std::endl;
        return EXIT_FAILURE;
    }

    return PrepareDatabase(argv[1]);
}
