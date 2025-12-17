#include "BitcoinExchange.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

int main(int argc, char *argv[]) {
    try { 
        if (argc != 2) throw std::invalid_argument(FILEERROR);
        BitcoinExchange btc;
        std::string path = static_cast<std::string>(argv[1]);

        btc.exchange(path);
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
