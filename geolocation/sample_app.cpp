#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <optional>

using Database = std::vector<char>;

Database LoadDatabase() {
    // Pretending to use memory
    Database database(1024 * 1024 * 5);
    for (long i = 0; i < database.size(); i++) {
        database[i] = 0xfe;
    }

    // Pretending this actually takes longer than it does..
    std::this_thread::sleep_for(std::chrono::milliseconds(3333));

    return database;
}

std::string PerformLookup(std::string ip) {
    // Pretending to perform the lookup
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return "US,New York";
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "error: usage './sample_app <database.csv>'" << std::endl;
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
            if (!database.has_value()){
                std::cerr << "error: Lookup requested before database was ever loaded" << std::endl;
                return EXIT_FAILURE;
            }

            std::cout << PerformLookup(cmd.substr(7)) << std::endl;

      } else if (cmd.find("EXIT") == 0) {
            std::cout << "OK" << std::endl;
            break;
      } else {
            std::cerr << "error: Unknown command received" << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
