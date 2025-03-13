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

constexpr size_t NUM_MAX = 10;
constexpr size_t NAME_MAX = 20;
constexpr size_t USERNAME_MAX = 254;
constexpr size_t PASSWORD_MAX = 32;
constexpr size_t PASSWORD_MIN = 8;

// defines the Str_pair type 
typedef std::pair<std::string, std::string> Str_pair;

// defines the Attendance_vector type to load the attendance data 
typedef std::vector<std::tuple<std::string, std::string, std::string>> Attendance_vector;

#endif
