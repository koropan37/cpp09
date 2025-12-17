#ifndef BITCOINEXCHANGE_HPP_
#define BITCOINEXCHANGE_HPP_

#include <map>
#include <string>
#include <ctime>
#include <fstream>

#define FILEERROR "Error: could not open file."

class BitcoinExchange {
 private:
    typedef std::map<time_t, double> Database;
    Database data_;

    BitcoinExchange(const BitcoinExchange& other); 
    BitcoinExchange& operator=(const BitcoinExchange& other);

 public:
    BitcoinExchange();
    ~BitcoinExchange();

    void inputCSV(std::ifstream& ifs);
    void exchange(const std::string& path);

    double getData(time_t time) const;
};

#endif
