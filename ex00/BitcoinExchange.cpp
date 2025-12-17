#include "BitcoinExchange.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring> 
#include <cstdlib>
#include <cerrno>

#define CSVMSG "date,exchange_rate"
#define CSVERROR "Error: invalid csv line: "

BitcoinExchange::BitcoinExchange() {
    std::ifstream ifs ("data.csv");
    if(!ifs) throw std::runtime_error("csv file is not opened");
    inputCSV(ifs);
}

BitcoinExchange::~BitcoinExchange() {}

static time_t parseDate(const std::string& date_str) {
    struct tm tm;
    std::memset(&tm, 0, sizeof(tm)); 
    // 文字列をフォーマットに代入
    char *res = strptime(date_str.c_str(), "%Y-%m-%d", &tm);
    if (res == NULL || *res != '\0')
        throw std::runtime_error("invalid date format: " + date_str);

    int year = tm.tm_year, 
        month = tm.tm_mon, 
        mday = tm.tm_mday;
    tm.tm_isdst = -1;
    // 厳密なチェック
    time_t t = mktime(&tm);
    if (t == (time_t)-1)
        throw std::runtime_error("mktime failed for date: " + date_str);
    if(year != tm.tm_year || 
       month != tm.tm_mon || 
       mday != tm.tm_mday) 
        throw std::runtime_error("Nonexistent: " + date_str);
    return t;
}

static bool parseRate(const std::string& s, double& out) {
    char *end = NULL;
    errno = 0;
    out = std::strtod(s.c_str(), &end);
    if (end == s.c_str() ||    
        *end != '\0'     ||    
        errno == ERANGE  ||
        out < 0.0) 
            return false; 
    return true;
}

// 前後の空白の除去
static std::string trim(std::string &s) {
    size_t a = s.find_first_not_of(" \t\r");
    if (a == std::string::npos) 
        return ""; 
    size_t b = s.find_last_not_of(" \t\r");
    return s.substr(a, b - a + 1);
}

void BitcoinExchange::inputCSV(std::ifstream& ifs) {
    std::string line;
    if (!std::getline(ifs, line))
        throw std::runtime_error("csv file is empty");
    if (trim(line) != CSVMSG)
        throw std::runtime_error(CSVERROR + line);

    while (std::getline(ifs, line)) {
        if (trim(line).empty()) 
            throw std::runtime_error("csv data is empty");

        std::string::size_type pos = line.find(',');
        if (pos == std::string::npos)
            throw std::runtime_error(CSVERROR + line);

        std::string date_str = line.substr(0, pos);
        std::string rate_str = line.substr(pos + 1);
        date_str = trim(date_str);
        rate_str = trim(rate_str);
        if (date_str.empty() || rate_str.empty())
            throw std::runtime_error(CSVERROR + line);

        time_t t = parseDate(date_str);
        if (data_.count(t)) // 日付の重複エラー
            throw std::runtime_error("duplicate date: " + date_str);

        double rate;
        if (!parseRate(rate_str, rate))
            throw std::runtime_error("invalid rate value: " + rate_str);
       
        data_[t] = rate;
    }
}

double BitcoinExchange::getData(time_t time) const {
    Database::const_iterator it = data_.upper_bound(time);
    if(it == data_.begin()) throw std::out_of_range("data is empty");
    return (--it)->second;
}

void BitcoinExchange::exchange(const std::string& path) {

}
