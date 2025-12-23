#include "PmergeMe.hpp"
#include <stdexcept>
#include <limits>
#include <cerrno>
#include <cstdlib>

PmergeMe::PmergeMe(int argc, char *argv[]) {
    parseNum(argc, argv);
}

static unsigned int validateNum(const char *argv);

void PmergeMe::parseNum(int argc, char *argv[]) {
    if(argc < 2) 
        throw std::runtime_error("Usage: ./PmergeMe n1 n2 ...");
    data_.reserve(argc - 1);
    for(int i = 1;  i < argc; ++i) {
        data_.push_back(validateNum(argv[i]));
    }  
}

static unsigned int validateNum(const char *argv) {
    if(argv == NULL || argv[0] == '\0' ||
        (!std::isdigit(static_cast<unsigned char>(argv[0]))))
        throw std::runtime_error("invalid argument");

    char *end = NULL;
    errno = 0;
    unsigned long tmp = std::strtoul(argv, &end, 10);
    if (end == argv || *end != '\0' || errno == ERANGE ||
        tmp > std::numeric_limits<unsigned int>::max())
        throw std::runtime_error("invalid argument");

    return static_cast<unsigned int>(tmp);
}