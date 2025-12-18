#ifndef RPN_HPP_
#define RPN_HPP_

#include <stack>
#include <string>

// enum OPERATOR {
//     ADD,
//     SUB,
//     MUL,
//     DIV
// };

class RPN {
 private:
    std::stack<int> numbers_;

    void add(int n1, int n2);
    void sub(int n1, int n2);
    void mul(int n1, int n2);
    void div(int n1, int n2);
    void validateChar(const char& c, bool& need_space);

    RPN(const RPN& other);
    RPN& operator=(const RPN& other);

 public:
    RPN();
    ~RPN();
    void calculate(const std::string& exp);
   
};

#endif
