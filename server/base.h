#pragma once
#include <map>
#include <string>

class base
{
private:
    std::map<std::string, std::string> data_base; 

public:
    
    void connect(std::string f);

    std::map<std::string, std::string> get_data()
    {
        return data_base;
    }

    bool has_login(const std::string& login) const
    {
        return data_base.find(login) != data_base.end();
    }

    
    std::string get_password(const std::string& login) const
    {
        auto it = data_base.find(login);
        if (it != data_base.end()) {
            return it->second;
        }
        return "";
    }
};
