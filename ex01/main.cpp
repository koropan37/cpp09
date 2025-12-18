#include "RPN.hpp"
#include <iostream>
#include <stdexcept>

int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cerr << "Usage: ./RPN <expression>" <<std::endl;
    }
    try {
        RPN rpn;
        rpn.calculate(static_cast<const std::string>(argv[1]));
        return 0;
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
