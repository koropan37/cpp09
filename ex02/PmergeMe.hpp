#ifndef PMERGEME_HPP_
#define PMERGEME_HPP_

#include <vector>
#include <deque>
#include <string>

class PmergeMe {
 public:
    PmergeMe(int argc, char *argv[]);
    ~PmergeMe();
 private:
    std::vector<unsigned int> data_;

    void parseNum(int argc, char *argv[]);
};

#endif