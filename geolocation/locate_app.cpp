#include <iostream>
#include <cstring>
#include <algorithm>

#include <arpa/inet.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include "definitions.h"

unsigned char* LoadDatabase(std::string filePath, uint32_t& recordCount) {
    int fd = open(filePath.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "error: can't open database file:" << filePath << std::endl;
        return 0;
    }

    // mmap and read db count record first
    unsigned char* data = (unsigned char*)mmap(NULL, DB_COUNT_RECORD_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        std::cerr << "error: can't mmap database record count" << std::endl;
        close(fd);
        return 0;
    }

    recordCount = *(uint32_t*)data;

    if (munmap(data, DB_COUNT_RECORD_SIZE)) {
        std::cerr << "error: can't munmap database record count" << std::endl;
    }

    // mmap complete db
    data = (unsigned char*)mmap(NULL, DB_COUNT_RECORD_SIZE + recordCount * (DB_INDEX_RECORD_SIZE + DB_LOCATION_RECORD_SIZE)
            , PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        std::cerr << "error: can't mmap database" << std::endl;
        close(fd);
        return 0;
    }

    close(fd);

    return data;
}

void CloseDatabase(unsigned char* database, uint32_t recordCount) {
    if (munmap(database, DB_COUNT_RECORD_SIZE + recordCount * (DB_INDEX_RECORD_SIZE + DB_LOCATION_RECORD_SIZE))) {
        std::cerr << "error: can't munmap database" << std::endl;
    }
}

std::string PerformLookup(std::string ip, uint32_t* data, uint32_t recordCount) {
    uint32_t address = inet_network(ip.c_str());
    // binary search the sorted db index
    auto upper = std::upper_bound(data, data + recordCount, address);
    upper--;

    char location[DB_LOCATION_RECORD_SIZE];
    uint32_t record = std::distance(data, upper);
    // calculate db record offset
    uint32_t offset = (recordCount * DB_INDEX_RECORD_SIZE) + (record * DB_LOCATION_RECORD_SIZE);

    memcpy(location, (unsigned char*)data + offset, sizeof(location));

    return std::string(location);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "error: usage './locate <database.csv>'" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "READY" << std::endl;

    unsigned char* data = 0;
    uint32_t recordCount = 0;
    int result = EXIT_SUCCESS;

    for (std::string cmd; std::getline(std::cin, cmd);) {
        if (cmd.find("LOAD") == 0) {
            std::string filePath(std::string(argv[1]) + ".bin");
            data = LoadDatabase(filePath.c_str(), recordCount);
            if (data) {
                std::cout << "OK" << std::endl;
            } else {
                result = EXIT_FAILURE;
                break;
            }
        } else if (cmd.find("LOOKUP") == 0) {
            if (!data) {
                std::cerr << "error: lookup requested before database and index was ever loaded" << std::endl;
                result = EXIT_FAILURE;
                break;
            }
            // offset by db count record
            uint32_t offset = DB_COUNT_RECORD_SIZE / sizeof(uint32_t);
            std::cout << PerformLookup(cmd.substr(7), (uint32_t*)data + offset, recordCount) << std::endl;
        } else if (cmd.find("EXIT") == 0) {
            std::cout << "OK" << std::endl;
            result = EXIT_SUCCESS;
            break;
        } else {
            std::cerr << "error: unknown command received" << std::endl;
            result = EXIT_FAILURE;
            break;
        }
    }

    CloseDatabase(data, recordCount);

    return result;
}
