#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <optional>
#include <arpa/inet.h>
#include <algorithm>
#include <byteswap.h>

using Database = std::vector<char>;

Database LoadDatabase() {
    return Database();
}

std::vector<std::string> DatabaseLine(std::string line) {
    std::string cell;
    std::stringstream lineStream(line);
    std::vector<std::string> result;

    while (std::getline(lineStream, cell, ',')) {
        result.push_back(cell);
    }

    return result;
}

std::string PerformLookup(std::string ip, std::string path) {
    std::ifstream databaseFile(path);
    if (!databaseFile) {
        std::cerr << "error: Can't open database file:" << path << std::endl;
    }

    struct in_addr ip_addr;
    inet_aton(ip.c_str(), &ip_addr);
    long address = bswap_32(ip_addr.s_addr);

    std::string line;
    while (std::getline(databaseFile, line)) {
        std::vector<std::string> result = DatabaseLine(line);

        std::string column0 = result[0];
        std::string column1 = result[1];
        column0.erase(std::remove(column0.begin(), column0.end(), '"'), column0.end());
        column1.erase(std::remove(column1.begin(), column1.end(), '"'), column1.end());

        struct in_addr addressStart;
        struct in_addr addressEnd;
        addressStart.s_addr = std::stol(column0.c_str());
        addressEnd.s_addr = std::stol(column1.c_str());

        if ((address >= addressStart.s_addr) && (address <= addressEnd.s_addr)) {
            std::string column3 = result[2];
            std::string column6 = result[5];
            column3.erase(std::remove(column3.begin(), column3.end(), '"'), column3.end());
            column6.erase(std::remove(column6.begin(), column6.end(), '"'), column6.end());

            return column3 + std::string(",") + column6;
        }
    }

    return std::string();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "error: usage './locate <database.csv>'" << std::endl;
        return EXIT_FAILURE;
    }

    // Indicate that the app is ready
    std::cout << "READY" << std::endl;

    std::optional<Database> database;
    for (std::string cmd; std::getline(std::cin, cmd);) {
        if (cmd.find("LOAD") == 0) {
            database = LoadDatabase();
            std::cout << "OK" << std::endl;
        } else if (cmd.find("LOOKUP") == 0) {
            if (!database.has_value()) {
                std::cerr << "error: Lookup requested before database was ever loaded" << std::endl;
                return EXIT_FAILURE;
            }
            std::cout << PerformLookup(cmd.substr(7), argv[1]) << std::endl;
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
