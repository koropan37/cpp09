#include "PmergeMe.hpp"
#include <stdexcept>
#include <limits>
#include <cerrno>
#include <cstdlib>
#include <time.h>
#include <stdint.h>

uint64_t getTime() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * 1000000000ULL +
           static_cast<uint64_t>(ts.tv_nsec);
}
PmergeMe::PmergeMe(int argc, char *argv[]) {
    
    parseNum(argc, argv);
}

static unsigned int validateNum(const char *argv);

void PmergeMe::parseNum(int argc, char *argv[]) {
    if(argc < 2) 
        throw std::runtime_error("Usage: ./PmergeMe n1 n2 ...");
    vec_.reserve(argc - 1);
    for(int i = 1;  i < argc; ++i) {
        vec_.push_back(validateNum(argv[i]));
        deq_.push_back(validateNum(argv[i]));
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

