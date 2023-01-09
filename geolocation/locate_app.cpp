#include <iostream>

#include <arpa/inet.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include "csv.h"
#include "definitions.h"

unsigned char* LoadDatabase(std::string filePath) {
    int fd = open(filePath.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "error: can't open database file:" << filePath << std::endl;
        return 0;
    }

    unsigned char* data = (unsigned char*)mmap(NULL, NR_OF_RECORDS * DB_RECORD_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        std::cerr << "error: can't mmap database" << std::endl;
        close(fd);
        return 0;
    }

    close(fd);

    return data;
}

void CloseDatabase(unsigned char* database) {
    if (munmap(database, NR_OF_RECORDS * DB_RECORD_SIZE)) {
        std::cerr << "error: can't mummap database" << std::endl;
    }
}

std::string PerformLookup(std::string ip, uint32_t* data) {
    uint32_t address = inet_network(ip.c_str());
    auto upper = std::upper_bound(data, data + NR_OF_RECORDS, address);
    upper--;

    int record = std::distance(data, upper);
    char location[DB_RECORD_SIZE - sizeof(uint32_t)];
    uint32_t offset = NR_OF_RECORDS * sizeof(uint32_t) + (record * (DB_RECORD_SIZE - sizeof(uint32_t)));

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
    int result = EXIT_SUCCESS;

    for (std::string cmd; std::getline(std::cin, cmd);) {
        if (cmd.find("LOAD") == 0) {
            std::string filePath(std::string(argv[1]) + ".bin");
            data = LoadDatabase(filePath.c_str());
            std::cout << "OK" << std::endl;
        } else if (cmd.find("LOOKUP") == 0) {
            if (!data) {
                std::cerr << "error: lookup requested before database and index was ever loaded" << std::endl;
                result = EXIT_FAILURE;
                break;
            }
            std::cout << PerformLookup(cmd.substr(7), (uint32_t*)data) << std::endl;
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

    CloseDatabase(data);

    return result;
}
