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

   struct Pair {
      unsigned int large_idx;   
      unsigned int small_idx;  
      bool inserted;         
   };

   std::vector<unsigned int> vec_;
   std::vector<Pair> largeVec_;
   std::vector<Pair> smallVec_;

   std::deque<unsigned int> deq_;
   std::deque<Pair> largeDeq_;
   std::deque<Pair> smallDeq_;


   void parseNum(int argc, char *argv[]);
};

#endif