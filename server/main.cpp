#include <iostream>
#include "base.h"
#include "interface.h"
#include "communicator.h"
#include "calc.h"
#include "error.h"
#include "log.h"

int main(int argc, const char** argv) {
    interface z;
    if (!z.parser(argc, argv)) {
        return 1;
    }
	string basefile = z.get_base();
	string logfile = z.get_log();
    z.setup_connection(basefile, logfile);
        
    return 0; 
}
