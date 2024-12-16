#pragma once
#include <system_error>
#include <stdexcept>
#include <string>
#include <cstdlib>

class crit_err:public std::runtime_error
{
	public:
		crit_err(const std::string& s):std::runtime_error(s){}
		
};

class no_crit_err:public std::runtime_error
{
	public:
		no_crit_err(const std::string s): std::runtime_error(s){}
};
