#ifndef PMERGEME_HPP_
#define PMERGEME_HPP_

#include <vector>
#include <deque>
#include <list>
#include <string>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <typeinfo>
#include <iomanip>

#define JACOBSTHAL_SIZE 20
#define NO_PAIR -1

class PmergeMe {
public:
    typedef struct {
        int index_;
        int pairIndex_;
    } Pair;

    PmergeMe(int argc, char *argv[]);
    ~PmergeMe();

    void sortVector();
    void sortDeque();

    void printAfter() const;
    
    size_t getVectorComp() const { return vecComp_; }
    size_t getDequeComp() const { return deqComp_; }

private:
    std::vector<unsigned int> vecContainer_;
    std::deque<unsigned int> deqContainer_;
    
    size_t vecComp_;
    size_t deqComp_;
    
    int Jacobsthal_[JACOBSTHAL_SIZE];  // 配列 → vectorに変更

    void parseNum(int argc, char *argv[]);
    void initJacobsthal();
    int numOfSquared(int n) const;

    template<typename Container, typename PairContainer>
    void mergeInsertionSort(Container& container, PairContainer& LargeSeq, 
                           PairContainer& SmallSeq, size_t& comp, int depth);
    
    template<typename Container, typename PairContainer>
    void initLargeSeq(const Container& container, PairContainer& LargeSeq,
                      int elementSize, int pairSize);
    
    template<typename Container, typename PairContainer>
    void initSmallSeq(const Container& container,
                      PairContainer& SmallSeq, int elementSize, int pairSize);
    
    template<typename Container, typename PairContainer>
    void insertSmallElement(Container& container, PairContainer& LargeSeq,
                          PairContainer& SmallSeq, size_t& comp,
                          int smallIndex, int elementSize);
    
    template<typename Container, typename PairContainer>
    int binarySearchLargeSeq(const Container& container, const PairContainer& LargeSeq,
                             const PairContainer& SmallSeq, size_t& comp,
                             int smallIndex);
    
    template<typename PairContainer>
    int searchInsertPosition(const PairContainer& LargeSeq, const PairContainer& SmallSeq,
                         int largeSeqPos, int elementSize);
    
    template<typename PairContainer>
    void updateLargeSeq(PairContainer& LargeSeq, const PairContainer& SmallSeq,
                     int largeSeqPos, int smallIndex, int elementSize);
    
    template<typename PairContainer>
    void updateSmallSeq(PairContainer& SmallSeq, int insertPos, int smallIndex, int elementSize);
    
    template<typename PairContainer>
    void resetLargeSeq(PairContainer& LargeSeq, int groupSize);
    
    template<typename Container>
    void rotateRange(Container& container, int first, int middle, int last);
    
    template<typename Container>
    void moveElements(Container& container, int groupStart, int groupEnd, int dest);

    template<typename Container>
    unsigned int getElement(const Container& container, int index);

    template<typename PairContainer>
    int getGroupStartPos(const PairContainer& container, int index, int elementSize);
    
    template<typename PairContainer>
    int getGroupEndPos(const PairContainer& container, int index);

    PmergeMe(); 
    PmergeMe(const PmergeMe& src); 
    PmergeMe& operator=(const PmergeMe& src); 
};

void printBefore(int argc, char *argv[]);
unsigned long getTime(); 
void printTimeVector(int argc, unsigned long start, unsigned long end, const PmergeMe& pm);
void printTimeDeque(int argc, unsigned long start, unsigned long end, const PmergeMe& pm);

#endif
