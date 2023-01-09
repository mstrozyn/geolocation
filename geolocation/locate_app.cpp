#include <vector>
#include <iostream>
#include <fstream>

#include <arpa/inet.h>

#include "csv.h"
#include "definitions.h"

using Index = std::vector<uint32_t>;

void LoadDatabase(std::fstream& databaseFile, Index& dataIndex) {
    uint32_t ip;
    int index = 1;

    while (databaseFile.good()) {
        databaseFile.read((char *)&ip, sizeof(uint32_t));
        dataIndex.push_back(ip);
        databaseFile.seekg(index * DB_RECORD_SIZE);
        index++;
    }

    databaseFile.clear();
    databaseFile.seekg(0);
}

std::string PerformLookup(std::string ip, std::fstream& databaseFile, const Index& index) {
    uint32_t address = inet_network(ip.c_str());
    auto upper = std::upper_bound(index.begin(), index.end(), address);
    upper--;

    int record = std::distance(index.begin(), upper);
    char data[DB_RECORD_SIZE - sizeof(uint32_t)];
    databaseFile.seekg((record * DB_RECORD_SIZE) + sizeof(uint32_t));
    databaseFile.read((char *)data, sizeof(data));

    return std::string(data);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "error: usage './locate <database.csv>'" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "READY" << std::endl;

    Index index;
    bool databaseLoaded = false;
    std::string filePath(std::string(argv[1]) + ".bin");
    std::fstream databaseFile(filePath, std::ios::in | std::ios::binary);
    if (!databaseFile) {
        std::cerr << "error: Can't open database file:" << filePath << std::endl;
    }

    for (std::string cmd; std::getline(std::cin, cmd);) {
        if (cmd.find("LOAD") == 0) {
            LoadDatabase(databaseFile, index);
            databaseLoaded = true;
            std::cout << "OK" << std::endl;
        } else if (cmd.find("LOOKUP") == 0) {
            if (!databaseLoaded) {
                std::cerr << "error: Lookup requested before database and index was ever loaded" << std::endl;
                return EXIT_FAILURE;
            }
            std::cout << PerformLookup(cmd.substr(7), databaseFile, index) << std::endl;
        } else if (cmd.find("EXIT") == 0) {
            std::cout << "OK" << std::endl;
            if (databaseLoaded) {
                databaseFile.close();
            }
            return EXIT_SUCCESS;
        } else {
            if (databaseLoaded) {
                databaseFile.close();
            }
            std::cerr << "error: Unknown command received" << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
