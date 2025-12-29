#include "PmergeMe.hpp"
#include <limits>
#include <cerrno>
#include <cstdlib>
#include <time.h>
#include <stdint.h>

PmergeMe::PmergeMe(int argc, char *argv[]) 
    : vecComp_(0), deqComp_(0) {
    parseNum(argc, argv);
    initJacobsthal();
}

PmergeMe::~PmergeMe() {}

static unsigned int validateNum(const char *s) {
    if (s == NULL || s[0] == '\0' || 
       !std::isdigit(static_cast<unsigned char>(s[0])))
        throw std::runtime_error("invalid argument");

    char *end = NULL;
    errno = 0;
    unsigned long tmp = std::strtoul(s, &end, 10);
    if (end == s || *end != '\0' || errno == ERANGE ||
        tmp > std::numeric_limits<unsigned int>::max())
        throw std::runtime_error("invalid argument");

    return static_cast<unsigned int>(tmp);
}

void PmergeMe::parseNum(int argc, char *argv[]) {
    if (argc < 2)
        throw std::runtime_error("Usage: ./PmergeMe n1 n2 ...");
    for (int i = 1; i < argc; ++i) {
        unsigned int num = validateNum(argv[i]);
        vecContainer_.push_back(num);
        deqContainer_.push_back(num);
    }
}

void PmergeMe::initJacobsthal() {
    Jacobsthal_[0] = 2;
    Jacobsthal_[1] = 2;
    for (int i = 2; i < JACOBSTHAL_SIZE; ++i) {
        Jacobsthal_[i] = Jacobsthal_[i - 1] + 2 * Jacobsthal_[i - 2];
    }
}

void PmergeMe::sortVector() {
    vecComp_ = 0;
    std::vector<Pair> vecLargeSeq, vecSmallSeq;
    mergeInsertionSort(vecContainer_, vecLargeSeq, vecSmallSeq, 
                      vecComp_, 0);
}

void PmergeMe::sortDeque() {
    deqComp_ = 0;
    std::deque<Pair> deqLargeSeq, deqSmallSeq;
    mergeInsertionSort(deqContainer_, deqLargeSeq, deqSmallSeq, 
                      deqComp_, 0);
}

template<typename Container, typename PairContainer>
void PmergeMe::mergeInsertionSort (Container& container, 
                                   PairContainer& LargeSeq, 
                                   PairContainer& SmallSeq, 
                                   size_t& comp, int depth) {

    int elementSize = numOfSquared(depth); 
    if (elementSize > static_cast<int>(container.size()) / 2)
        return;
    
    int pairSize = elementSize * 2; //再起でのペアのサイズ
    
    for (int i = elementSize - 1; //group内の最大の要素
         i + elementSize < static_cast<int>(container.size()); 
         i += pairSize) {
        comp++;
        if (container[i] > container[i + elementSize]) {
            rotateRange(container, i - (elementSize - 1), i + 1, i + 1 + elementSize);
        } 
    }
    
    mergeInsertionSort(container, LargeSeq, SmallSeq, comp, depth + 1);
    
    initLargeSeq(container, LargeSeq, elementSize, pairSize);
    initSmallSeq(container, SmallSeq, elementSize, pairSize);
    
    int jacobsthalIndex = 0;
    while (!SmallSeq.empty()) {
        if (jacobsthalIndex >= JACOBSTHAL_SIZE)
            throw std::runtime_error("Jacobsthal overflow");
        
        int groupSize = Jacobsthal_[jacobsthalIndex];
        if (groupSize > static_cast<int>(SmallSeq.size())) {
            groupSize = static_cast<int>(SmallSeq.size());
        }
        
        for (int groupIndex = groupSize - 1; groupIndex >= 0; --groupIndex) {
            insertSmallElement(container, LargeSeq, SmallSeq, comp, 
                            groupIndex, elementSize);
        }
        
        SmallSeq.erase(SmallSeq.begin(), SmallSeq.begin() + groupSize);
        resetLargeSeq(LargeSeq, groupSize);
        
        ++jacobsthalIndex;
    }
}

int PmergeMe::numOfSquared(int n) const {
    //再起の深さの測定
    if (n >= std::numeric_limits<int>::max()) {
        throw std::runtime_error("Error: numOfSquared overflow");
    }
    return 1 << n;
}

template<typename Container>
void PmergeMe::rotateRange(Container& container, int first, int middle, int last) {
    // 前後の要素を比べて必要なら要素全てを rotate
    std::rotate(container.begin() + first, 
                container.begin() + middle,
                container.begin() + last);
}

template<typename Container, typename PairContainer>
void PmergeMe::initLargeSeq(const Container& container, 
                            PairContainer& LargeSeq,
                            int elementSize, int pairSize) {
    int size = static_cast<int>(container.size());
    LargeSeq.clear();
    
    if (elementSize <= size) { //初めの大きい要素のペア
        Pair tmp = {elementSize - 1, NO_PAIR};
        LargeSeq.push_back(tmp);
    } 
    if (pairSize <= size) { // 初めの大きい要素
        Pair tmp = {pairSize - 1, NO_PAIR};
        LargeSeq.push_back(tmp);
    }
    
    int pairsmallIndex = 0; 
    for (int i = pairSize * 2 - 1; i < size; i += pairSize) {
        Pair tmp = {i, pairsmallIndex};
        LargeSeq.push_back(tmp);//smallseq[0]... とペア
        ++pairsmallIndex;
    }
}

template<typename Container, typename PairContainer>
void PmergeMe::initSmallSeq(const Container& container,
                            PairContainer& SmallSeq, 
                            int elementSize, int pairSize) {
    int size = static_cast<int>(container.size());
    SmallSeq.clear();
    
    int pairMainIndex = 2;
    for (int i = pairSize + elementSize - 1; i < size; i += pairSize) {
        if (i + elementSize < size) { //ペアの小さい要素(Largeseq[2]... とペア)
            Pair tmp = {i, pairMainIndex}; 
            SmallSeq.push_back(tmp);
        } else {                    //ペアが作れない余り
            Pair tmp = {i, NO_PAIR}; 
            SmallSeq.push_back(tmp);
        }
        ++pairMainIndex;
    }
}

template<typename Container, typename PairContainer>
void PmergeMe::insertSmallElement(Container& container, 
                                  PairContainer& LargeSeq, 
                                  PairContainer& SmallSeq, size_t& comp,
                                  int smallIndex, int elementSize) {

    int largeSeqPos = binarySearchLargeSeq(container, LargeSeq, SmallSeq, 
                                             comp, smallIndex);
    int insertPos = searchInsertPosition(LargeSeq, SmallSeq, largeSeqPos, elementSize);
    
    int groupStart = getGroupStartPos(SmallSeq, smallIndex, elementSize);
    int groupEnd = getGroupEndPos(SmallSeq, smallIndex);
    
    moveElements(container, groupStart, groupEnd, insertPos);
    
    updateLargeSeq(LargeSeq, SmallSeq, largeSeqPos, smallIndex, elementSize);
    updateSmallSeq(SmallSeq, insertPos, smallIndex, elementSize);
    
    Pair entry = {insertPos + elementSize - 1, NO_PAIR};
    LargeSeq.insert(LargeSeq.begin() + largeSeqPos, entry);
}

template<typename Container, typename PairContainer>
int PmergeMe::binarySearchLargeSeq(const Container& container, 
                                   const PairContainer& LargeSeq, 
                                   const PairContainer& SmallSeq, 
                                   size_t& comp, int smallIndex) {
    int start = 0;
    int end = (SmallSeq[smallIndex].pairIndex_ == NO_PAIR) // pairの有無
              ? static_cast<int>(LargeSeq.size()) - 1 //なし(large全体)
              : SmallSeq[smallIndex].pairIndex_ - 1; //あり(ペアの前まで)
    
    while (start <= end) {
        int mid = start + (end - start) / 2;
        comp++;

        // 指定した値をLargeSeqの中間の値と比べる
        if (getElement(container, SmallSeq[smallIndex].index_) < 
            getElement(container, LargeSeq[mid].index_)) { 
            end = mid - 1;
        } else {
            start = mid + 1;
        }
    }
    return start;
}

template<typename PairContainer>
int PmergeMe::searchInsertPosition(const PairContainer& LargeSeq, 
                                const PairContainer& SmallSeq,
                                int largeSeqPos, int elementSize) {
    if (largeSeqPos < static_cast<int>(LargeSeq.size())) {
        if (LargeSeq[largeSeqPos].pairIndex_ == NO_PAIR) { //LargeSeq の[0], [1]
            return getGroupStartPos(LargeSeq, largeSeqPos, elementSize);
        } else {
            int pairIndex = LargeSeq[largeSeqPos].pairIndex_; // その他のペアの部分
            return getGroupStartPos(SmallSeq, pairIndex, elementSize);
        }
    } else {
        if (SmallSeq.empty()) {
            if (LargeSeq.empty()) {
                return 0;
            }
            return LargeSeq[LargeSeq.size() - 1].index_ + 1;
        }
        return getGroupStartPos(SmallSeq, SmallSeq.size() - 1, elementSize); //余りで全要素より大きい
    }
}

template<typename PairContainer>
int PmergeMe::getGroupStartPos(const PairContainer& container, int index, int elementSize) {
    return container[index].index_ - elementSize + 1;
}

template<typename PairContainer>
int PmergeMe::getGroupEndPos(const PairContainer& container, int index) {
    return container[index].index_ + 1;
}

template<typename Container> // 要素の移動
void PmergeMe::moveElements(Container& container, int groupStart, int groupEnd, int insertPos) {
    Container tmp(container.begin() + groupStart, container.begin() + groupEnd);
    container.erase(container.begin() + groupStart, container.begin() + groupEnd);
    container.insert(container.begin() + insertPos, tmp.begin(), tmp.end());
}

template<typename PairContainer>
void PmergeMe::updateLargeSeq(PairContainer& LargeSeq, const PairContainer& SmallSeq,
                           int largeSeqPos, int smallIndex, int elementSize) {
    int endPairPos = (SmallSeq[smallIndex].pairIndex_ == NO_PAIR)
                       ? static_cast<int>(LargeSeq.size())
                       : SmallSeq[smallIndex].pairIndex_;
    
    for (int i = largeSeqPos; i < endPairPos; ++i) {
        LargeSeq[i].index_ += elementSize;
    }
    
    if (SmallSeq[smallIndex].pairIndex_ != NO_PAIR)
        LargeSeq[SmallSeq[smallIndex].pairIndex_].pairIndex_ = NO_PAIR;
}

template<typename PairContainer>
void PmergeMe::updateSmallSeq(PairContainer& SmallSeq, int insertPos, 
                           int smallIndex, int elementSize) {
    int beginIndex = static_cast<int>(SmallSeq.size());
    for (int i = 0; i < static_cast<int>(SmallSeq.size()); ++i) {
        if (SmallSeq[i].index_ >= insertPos) {
            beginIndex = i;
            break;
        }
    }
    
    for (int i = beginIndex; i < static_cast<int>(SmallSeq.size()); ++i) {
        if (i < smallIndex) {
            SmallSeq[i].index_ += elementSize;
        }
        if (SmallSeq[i].pairIndex_ != NO_PAIR) {
            ++SmallSeq[i].pairIndex_;
        }
    }
    SmallSeq[smallIndex].pairIndex_ = NO_PAIR;
}

template<typename PairContainer>
void PmergeMe::resetLargeSeq(PairContainer& LargeSeq, int groupSize) {
    for (size_t i = 0; i < LargeSeq.size(); ++i) {
        if (LargeSeq[i].pairIndex_ != NO_PAIR)
            LargeSeq[i].pairIndex_ -= groupSize;
    }
}

template<typename Container>
unsigned int PmergeMe::getElement(const Container& container, int index) {
    return container[index];
}

unsigned long getTime() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<unsigned long>(ts.tv_sec) * 1000000UL +
           static_cast<unsigned long>(ts.tv_nsec) / 1000UL;
}

void printBefore(int argc, char *argv[]) {
    std::cout << "Before:\t";
    for(int i = 1; i < argc; ++i) {
      if(i <= 6)
        std::cout << argv[i] << " ";
      else {
        std::cout << "[...]";
        break;
      }
    }
    std::cout << std::endl; 
}

void PmergeMe::printAfter() const {
    std::cout << "After:\t";
    std::vector<unsigned int>::const_iterator it = vecContainer_.begin();
    int count = 0;
    while (it != vecContainer_.end() && count < 6) {
        std::cout << *it << " ";
        ++it;
        ++count;
    }
    if (it != vecContainer_.end())
        std::cout << "[...]";
    std::cout << std::endl;
}

void printTimeVector(int argc, unsigned long start, unsigned long end, const PmergeMe& pm) {
    double timeInMicroseconds = static_cast<double>(end - start);
    std::cout 
        << "Time to process a range of " 
        << argc - 1 
        << " elements with std::vector : " 
        << std::fixed << std::setprecision(5) << timeInMicroseconds << " us" << std::endl;
    std::cout << "comparison vector: " << pm.getVectorComp() << std::endl;
    std::cout << std::resetiosflags(std::ios::floatfield);
}

void printTimeDeque(int argc, unsigned long start, unsigned long end, const PmergeMe& pm) {
    double timeInMicroseconds = static_cast<double>(end - start);
    std::cout 
        << "Time to process a range of " 
        << argc - 1 
        << " elements with std::deque : " 
        << std::fixed << std::setprecision(5) << timeInMicroseconds << " us" << std::endl;
    std::cout << "comparison deque: " << pm.getDequeComp() << std::endl;
    std::cout << std::resetiosflags(std::ios::floatfield);
}

