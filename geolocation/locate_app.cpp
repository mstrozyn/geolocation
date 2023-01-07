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

        struct in_addr addressStart;
        struct in_addr addressEnd;
        addressStart.s_addr = std::stol(result[0].c_str());
        addressEnd.s_addr = std::stol(result[1].c_str());

        if ((address >= addressStart.s_addr) && (address <= addressEnd.s_addr)) {
            return result[2] + std::string(",") + result[5];
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
