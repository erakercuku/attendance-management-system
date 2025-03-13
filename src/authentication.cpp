#include "authentication.hpp"

std::string Auth::encrypt_password(const std::string &password)
{
    // to be properly implemented 

    std::string encrypted;

    size_t size = password.size();
    for (size_t i = 0; i < size; i++)
    {
        encrypted += password.at(i) + 5;
    }

    return encrypted;
}


bool Auth::verify_password(const std::string &inp_password, const std::string &encr_password)
{
    return encrypt_password(inp_password) == encr_password;
}
