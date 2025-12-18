#include "RPN.hpp"
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <limits>

RPN::RPN() : result_(0) {}

RPN::~RPN() {}

static bool validateSpace(const std::string& exp, size_t i, bool& need_space) {
    if(exp[i] != ' ' && exp[i] != '\t') 
        throw std::runtime_error("need space separator");
    if(i + 1 >= exp.length())
        throw std::runtime_error("No spaces at the end");
    need_space = false;
    return true; 
}

static bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

void RPN::add(int n1, int n2) {
    long tmp = static_cast<long>(n1) + static_cast<long>(n2);

    if(tmp > std::numeric_limits<int>::max()) throw std::overflow_error("overflow");
    if(tmp < std::numeric_limits<int>::min()) throw std::overflow_error("underflow");

    numbers_.push(static_cast<int>(tmp));
}
    
void RPN::sub(int n1, int n2) {
    long tmp = static_cast<long>(n1) - static_cast<long>(n2);

    if(tmp > std::numeric_limits<int>::max()) throw std::overflow_error("overflow");
    if(tmp < std::numeric_limits<int>::min()) throw std::overflow_error("underflow");
    
    numbers_.push(static_cast<int>(tmp));
}       

void RPN::mul(int n1, int n2) {
    long tmp = static_cast<long>(n1) * static_cast<long>(n2);

    if(tmp > std::numeric_limits<int>::max()) throw std::overflow_error("overflow");
    if(tmp < std::numeric_limits<int>::min()) throw std::overflow_error("underflow");
    
    numbers_.push(static_cast<int>(tmp));
}

void RPN::div(int n1, int n2) {
    if(n2 == 0) throw std::runtime_error("division by zero");

    long tmp = static_cast<long>(n1) / static_cast<long>(n2);

    if(tmp > std::numeric_limits<int>::max()) throw std::overflow_error("overflow");
    if(tmp < std::numeric_limits<int>::min()) throw std::overflow_error("underflow");
    
    numbers_.push(static_cast<int>(tmp));
}

void RPN::validateChar(const char c, bool& need_space) {
    need_space = true;
    if (std::isdigit(static_cast<unsigned char>(c)))
        numbers_.push(c - '0');
    else if(isOperator(c)) {
        if(numbers_.size() < 2) throw std::runtime_error("need more operands");
        int n2 = numbers_.top();
        numbers_.pop();
        int n1 = numbers_.top();
        numbers_.pop();
        if      (c == '+') add(n1, n2);
        else if (c == '-') sub(n1, n2);
        else if (c == '*') mul(n1, n2);
        else if (c == '/') div(n1, n2);
    }
    else throw std::runtime_error("unexpected character");
}

void RPN::calculate(const std::string& exp) {
    if(exp.empty()) throw std::runtime_error("Nothing Expression.");
    bool need_space = false;
    for (size_t i = 0; i < exp.length(); ++i) {
        if(need_space == true) {
            if(validateSpace(exp, i, need_space)) continue;                              
        }
        else 
            validateChar(exp[i], need_space);                
    } 
    if (numbers_.empty())
    throw std::runtime_error("no result");
    if (numbers_.size() != 1)
        throw std::runtime_error("too many operands");
    result_ = numbers_.top();
    numbers_.pop();
    std::cout << result_ << std::endl;
}
