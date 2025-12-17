#include "BitcoinExchange.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>

int main(int argc, char *argv[]) {
    try { 
        if (argc != 2) throw std::invalid_argument(OPENERR);
        BitcoinExchange btc;
        std::ifstream path(argv[1]);
        if(!path) throw std::invalid_argument(OPENERR);    
        btc.exchange(path);
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
