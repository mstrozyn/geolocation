#include <vector>
#include <iostream>
#include <fstream>

#include <arpa/inet.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "csv.h"
#include "definitions.h"

using Index = std::vector<uint32_t>;

uint32_t* LoadDatabase(std::string filePath) {
    int fd = open(filePath.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "error: Can't open database file:" << filePath << std::endl;
        return NULL;
    }

    uint32_t* index = (uint32_t*)mmap(NULL, NR_OF_RECORDS * sizeof(uint32_t), PROT_READ, MAP_PRIVATE, fd, 0);
    if (index == MAP_FAILED) {
        std::cerr << "error: Can't mmap database index:" << filePath << std::endl;
        return NULL;
    }

    return index;
}

std::string PerformLookup(std::string ip, std::fstream& databaseFile, uint32_t* index) {
    uint32_t address = inet_network(ip.c_str());
    auto upper = std::upper_bound(index, index + NR_OF_RECORDS, address);
    upper--;

    int record = std::distance(index, upper);
    char data[DB_RECORD_SIZE - sizeof(uint32_t)];
    uint32_t offset = NR_OF_RECORDS * sizeof(uint32_t) + (record * (DB_RECORD_SIZE - sizeof(uint32_t)));
    databaseFile.seekg(offset);
    databaseFile.read((char *)data, sizeof(data));

    offset = NR_OF_RECORDS * sizeof(uint32_t) + ((DB_RECORD_SIZE / 2) * (DB_RECORD_SIZE - sizeof(uint32_t)));
    databaseFile.seekg(offset);

    return std::string(data);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "error: usage './locate <database.csv>'" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "READY" << std::endl;

    uint32_t* index = NULL;
    std::string filePath(std::string(argv[1]) + ".bin");
    std::fstream databaseFile(filePath, std::ios::in | std::ios::binary);
    if (!databaseFile) {
        std::cerr << "error: Can't open database file:" << filePath << std::endl;
    }

    for (std::string cmd; std::getline(std::cin, cmd);) {
        if (cmd.find("LOAD") == 0) {
            index = LoadDatabase(filePath.c_str());
            std::cout << "OK" << std::endl;
        } else if (cmd.find("LOOKUP") == 0) {
            if (!index) {
                std::cerr << "error: Lookup requested before database and index was ever loaded" << std::endl;
                return EXIT_FAILURE;
            }
            std::cout << PerformLookup(cmd.substr(7), databaseFile, index) << std::endl;
        } else if (cmd.find("EXIT") == 0) {
            std::cout << "OK" << std::endl;
            if (index) {
                databaseFile.close();
            }
            return EXIT_SUCCESS;
        } else {
            if (index) {
                databaseFile.close();
            }
            std::cerr << "error: Unknown command received" << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
