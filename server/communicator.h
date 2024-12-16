#pragma once
#include <string>
#include <vector>
#include <map>
#include "log.h"
#include "error.h"

using namespace std;

class communicator
{
public:
    int connection(int port, std::map<std::string, std::string> base, logger* l);
    std::string sha224(std::string input_str);
};
