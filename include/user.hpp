#ifndef USER_HPP
#define USER_HPP

#include "utils.hpp"

enum Roles {ADMIN = 1, MANAGER, USER, ROLES_COUNT};

enum Login_opt
{
    LOGIN = 1,
    CREATE_ACCOUNT = 2,
    EXIT = 3,
    LOGIN_OPT_COUNT
};

class User
{
protected:
    Str_pair fullname;
    std::string username;
    std::string password;
    Roles role;

public:
    User(void) = default;
    User(const Str_pair &fullname, const std::string &username, 
         const std::string &password, const Roles role)
        : fullname(fullname), username(username), password(password), role(role) {}

    bool login(void);
    void create_account(void);

    Str_pair get_fullname(void) const { return fullname; };
    std::string get_username(void) const { return username; };
    std::string get_password(void) const { return password; };
    Roles get_role(void) const { return role; };
    std::string get_role_name(void) const;
};

#endif
