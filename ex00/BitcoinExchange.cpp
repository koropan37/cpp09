#include "BitcoinExchange.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring> 
#include <cstdlib>
#include <cerrno>

#define CSVMSG "date,exchange_rate"
#define FILEEMPTY "file is empty"
#define FILEERR "Error: invalid path line: "
#define CSVERR "Error: invalid csv line: "
#define MKTERR "Error: mktime failed for date: "
#define INPUTMSG "date | value"
#define BADINPUT "Error: bad input => "
#define DUPERR "Error: csv duplicate date: "

#define NEGATIVEERR "Error: not a positive number."
#define LARGEERR "Error: too large a number."

BitcoinExchange::BitcoinExchange() {
    std::ifstream ifs ("data.csv");
    if(!ifs) throw std::runtime_error("Error: could not open csv.");
    inputCsv(ifs);
}

BitcoinExchange::~BitcoinExchange() {}

// 前後の空白の除去
static std::string trim(const std::string &s) {
    size_t a = s.find_first_not_of(" \t\r");
    if (a == std::string::npos) 
        return ""; 
    size_t b = s.find_last_not_of(" \t\r");
    return s.substr(a, b - a + 1);
}

static void splitLine(
    const std::string& line, char delimiter,
    std::string& date_str, std::string& rate_str,
    const std::string& errMsg) {

    std::string::size_type pos = line.find(delimiter);
    if (pos == std::string::npos)
        throw std::runtime_error(errMsg + line);

    std::string left = line.substr(0, pos);
    std::string right = line.substr(pos + 1);
    date_str = trim(left);
    rate_str = trim(right);
    if (date_str.empty() || rate_str.empty())
        throw std::runtime_error(errMsg + line);
}

static time_t parseDate(const std::string& date_str) {
    struct tm tm;
    std::memset(&tm, 0, sizeof(tm)); 
    // 文字列をフォーマットに代入
    char *res = strptime(date_str.c_str(), "%Y-%m-%d", &tm);
    if (res == NULL || *res != '\0')
        throw std::runtime_error(std::string(BADINPUT) + date_str);

    int year = tm.tm_year, 
        month = tm.tm_mon, 
        mday = tm.tm_mday;
    tm.tm_isdst = -1;
    // 厳密なチェック
    time_t time = mktime(&tm);
    if (time == (time_t)-1)
        throw std::runtime_error(std::string(MKTERR) + date_str);
    if(year != tm.tm_year || 
       month != tm.tm_mon || 
       mday != tm.tm_mday) 
        throw std::runtime_error(std::string(BADINPUT) + date_str);
    return time;
}

static bool parseCsvRate(const std::string& rate_str, double& rate) {
    char *end = NULL;
    errno = 0;
    rate = std::strtod(rate_str.c_str(), &end);
    if (end == rate_str.c_str() ||    
        *end != '\0'     ||    
        errno == ERANGE  ||
        rate < 0.0) 
            return false; 
    return true;
}

void BitcoinExchange::inputCsv(std::ifstream& ifs) {
    std::string line;
    if (!std::getline(ifs, line))
        throw std::runtime_error("Error: csv " FILEEMPTY);
    if (trim(line) != CSVMSG)
        throw std::runtime_error(std::string(CSVERR) + line);

    while (std::getline(ifs, line)) {
        if (trim(line).empty()) 
            throw std::runtime_error("Error: csv " FILEEMPTY);

        std::string date_str, rate_str;
        splitLine(line, ',', date_str, rate_str, CSVERR);

        time_t time = parseDate(date_str);
        if (data_.count(time)) // 日付の重複エラー
            throw std::runtime_error(std::string(DUPERR) + date_str);

        double rate;
        if (!parseCsvRate(rate_str, rate))
            throw std::runtime_error(std::string(CSVERR) + rate_str);
       
        data_[time] = rate;
    }
}

static void parseInputRate(const std::string& rate_str, double& rate) {
        char *end = NULL;
        errno = 0;
        rate = std::strtod(rate_str.c_str(), &end);
        if (end == rate_str.c_str() || *end != '\0') 
            throw std::runtime_error(std::string(BADINPUT) + rate_str);
        if(rate < 0.0) 
            throw std::runtime_error(NEGATIVEERR);
        if(  errno == ERANGE  || rate > 1000.0)
            throw std::runtime_error(LARGEERR);
}

double BitcoinExchange::getData(time_t time) const {
    Database::const_iterator it = data_.upper_bound(time);
    if(it == data_.begin()) throw std::out_of_range("Error: data is empty");
    return (--it)->second;
}

void BitcoinExchange::printExchange(
    const std::string &date_str, 
    const std::string &rate_str, 
    time_t time, double rate) {
    std::cout 
        << date_str << " => " << rate_str << " = "
        << getData(time) * rate << std::endl;
}

void BitcoinExchange::exchange(std::ifstream& path) {
    std::string line;
    if (!std::getline(path, line))
        throw std::runtime_error("Error: input " FILEEMPTY);
    if (trim(line) != INPUTMSG)
        throw std::runtime_error(std::string(FILEERR) + line);
    while (std::getline(path, line)) {
        try {
            if (trim(line).empty()) continue;

        std::string date_str, rate_str;
        splitLine(line, '|', date_str, rate_str, BADINPUT);

        time_t time = parseDate(date_str);

        double rate;
        parseInputRate(rate_str, rate);
        
        printExchange(date_str, rate_str, time, rate);       
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
    } 

    }
}
