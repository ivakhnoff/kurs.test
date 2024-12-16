#pragma once
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "log.h"
#include "base.h"
#include "communicator.h"
#include "error.h"

class interface {
    int port; 
    string basefile;
    string logfile;

public:
    interface() : port(33333), basefile("base.txt"), logfile("log.txt") {} //< конструктор по умолчанию
   
    bool parser(int argc, const char** argv);
    
    void setup_connection(const std::string& basefile, const std::string& logfile);
    
    void spravka(const boost::program_options::options_description& opts);
    
    int get_port() const { return port; }
    std::string get_base() { return basefile;}
    std::string get_log() { return logfile;}
};
