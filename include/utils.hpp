#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <tuple>

// defines the macro used to set the precision of decimal places when printing a double 
#define FIXED_DOUBLE(x) std::fixed << std::setprecision(2) << (x)
// defines the macro used to clear the input buffer and prevent buffer overflow 
#define CLEAR_INPUT_BUFFER std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n')
// defines the macro used to determine if a given year is a leap year or not 
#define IS_LEAP_YEAR(year) ((year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))? true : false)

// set of named values for the months of the year 
enum Months {JAN = 1, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC};

constexpr size_t NUM_MAX = 10;
constexpr size_t NAME_MAX = 21;
constexpr size_t USERNAME_MAX = 254;
constexpr size_t USERNAME_MIN = 5;
constexpr size_t PASSWORD_MAX = 33;
constexpr size_t PASSWORD_MIN = 8;
constexpr size_t DATE_MAX = 11;
constexpr size_t LOGIN_MAX = 6;
constexpr size_t LOGIN_ATTEMPTS_MAX = 3;

// defines the Str_pair type 
using Str_pair = std::pair<std::string, std::string>;

#endif
