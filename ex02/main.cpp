#include "PmergeMe.hpp"
#include <iostream>
#include <stdexcept>

int main(int argc, char *argv[]) {
    try {
        PmergeMe(argc, argv);
        return 0;
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1; 
    }
}