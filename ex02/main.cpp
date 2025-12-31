#include "PmergeMe.hpp"
#include <iostream>
#include <stdexcept>

int main(int argc, char *argv[]) {
    try {
        PmergeMe pm(argc, argv);
        printBefore(argc, argv); 
        unsigned long start = getTime();
        pm.sortVector();
        unsigned long end = getTime();
        pm.printAfter();
        printTimeVector(argc, start, end, pm);
        
        start = getTime();
        pm.sortDeque();
        end = getTime();
        printTimeDeque(argc, start, end, pm);
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1; 
    }
    
    return 0;
}
