#include <vector>
#include <iostream>

#include <byteswap.h>
#include <arpa/inet.h>

#include "csv.h"

using Database = std::vector<std::string>;
using Index = std::vector<uint32_t>;

void LoadDatabase(std::string path, Database& data, Index& dataIndex) {
    io::CSVReader<3> in(path);
    std::string column1, column2, column3;

    while (in.read_row(column1, column2, column3)) {
        dataIndex.push_back(std::stol(column1));
        data.push_back(column2 + std::string(",") + column3);
    }
}

std::string PerformLookup(std::string ip, const Database& database, const Index& index) {
    struct in_addr ip_addr;
    inet_aton(ip.c_str(), &ip_addr);
    long address = bswap_32(ip_addr.s_addr);

    auto upper = std::upper_bound(index.begin(), index.end(), address);
    upper--;

    std::string result = database[std::distance(index.begin(), upper)];
    result.erase(std::remove(result.begin(), result.end(), '"'), result.end());

    return result;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "error: usage './locate <database.csv>'" << std::endl;
        return EXIT_FAILURE;
    }

    // app ready
    std::cout << "READY" << std::endl;

    Index index;
    index.reserve(2942966);
    Database database;
    database.reserve(2942966);
    bool databaseLoaded = false;

    for (std::string cmd; std::getline(std::cin, cmd);) {
        if (cmd.find("LOAD") == 0) {
            LoadDatabase(argv[1], database, index);
            databaseLoaded = true;
            std::cout << "OK" << std::endl;
        } else if (cmd.find("LOOKUP") == 0) {
            if (!databaseLoaded) {
                std::cerr << "error: Lookup requested before database and index was ever loaded" << std::endl;
                return EXIT_FAILURE;
            }
            std::cout << PerformLookup(cmd.substr(7), database, index) << std::endl;
        } else if (cmd.find("EXIT") == 0) {
            std::cout << "OK" << std::endl;
            return EXIT_SUCCESS;
        } else {
            std::cerr << "error: Unknown command received" << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
