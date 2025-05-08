#ifndef INPUT_HPP
#define INPUT_HPP

#include <string>

class Input
{
public:
    // input management methods 
    static void get(const std::string &prompt, std::string &input, size_t size, 
                    bool (*validate)(const std::string &));
    static void get(const std::string &prompt, std::string &input, size_t size, 
                    const int opt_num, bool (*validate)(const std::string &, const int));

    // validation-related methods 
    static bool valid_opt(const std::string &inp, const int opt_num);
    static bool valid_yes_or_no(const std::string &inp);
    static bool valid_name(const std::string &name);
    static bool valid_username(const std::string &username);
    static bool valid_password(const std::string &password);

    static bool is_username_unique(const std::string &username);
};

#endif
