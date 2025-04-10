#ifndef AUTHENTICATION_HPP
#define AUTHENTICATION_HPP

#include <string>

class Auth 
{
public:
    static std::string hash_password(const std::string &password);
    static bool verify_password(const std::string &inp_password, const std::string &hashed_password);
};

#endif
