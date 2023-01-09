#include <iostream>
#include <fstream>

#include "csv.h"
#include "definitions.h"

void PrepareDatabase(std::string path) {
    io::CSVReader<8, io::trim_chars<' '>, io::double_quote_escape<',','\"'> > ipReader(path);
    std::string column1, column2, column3, column4, column5, column6, column7, column8;

    std::string filePath(path + ".bin");
    std::fstream databaseFile(filePath, std::ios::out | std::ios::binary);
    if (!databaseFile) {
        std::cerr << "error: Can't create database file:" << filePath << std::endl;
    }

    while (ipReader.read_row(column1, column2, column3, column4, column5, column6, column7, column8)) {
        uint32_t ip = std::stol(column1);
        databaseFile.write((const char *)&ip, sizeof(uint32_t));
    }

    io::CSVReader<8, io::trim_chars<' '>, io::double_quote_escape<',','\"'> > recordReader(path);
    while (recordReader.read_row(column1, column2, column3, column4, column5, column6, column7, column8)) {
        std::string location = column3 + std::string(",") + column6;
        databaseFile.write(location.c_str(), DB_RECORD_SIZE - sizeof(uint32_t));
    }

    databaseFile.close();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "error: usage './locate <database.csv>'" << std::endl;
        return EXIT_FAILURE;
    }

    PrepareDatabase(argv[1]);

    return EXIT_SUCCESS;
}
