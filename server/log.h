#pragma once
#include <string>

class logger {
private:
	
    
    std::string path_to_logfile; 

public:
    logger(); 

    logger(const std::string& path); 

    
    int set_path(const std::string& path_file);

   
    int writelog(const std::string& message);
    
    std::string gettime();

    std::string get_path() const;
};
