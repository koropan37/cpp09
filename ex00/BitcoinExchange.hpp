#ifndef BITCOINEXCHANGE_HPP_
#define BITCOINEXCHANGE_HPP_

#include <map>
#include <string>
#include <ctime>
#include <fstream>

#define OPENERR "Error: could not open file."

class BitcoinExchange {
 private:
    typedef std::map<time_t, double> Database;
    Database data_;

    BitcoinExchange(const BitcoinExchange& other); 
    BitcoinExchange& operator=(const BitcoinExchange& other);

 public:
    BitcoinExchange();
    ~BitcoinExchange();

    void inputCsv(std::ifstream& ifs);
    void exchange(std::ifstream& path);
    
    void printExchange(const std::string &date_str, 
                       const std::string &rate_str, 
                       time_t time, double rate);
    double getData(time_t time) const;
};

#endif
