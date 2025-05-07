#ifndef AUTHENTICATION_HPP
#define AUTHENTICATION_HPP

#include "user.hpp"
#include <memory>
#include <string>

class Auth 
{
public:
    static std::unique_ptr<User> login(void);
    static std::unique_ptr<User> create_account(void);

    static const std::string hash(const std::string &original);
    static bool verify_password(const std::string &inp_password, const std::string &hashed_password);
};

#endif
