#include <string.h>
#include "CmdArgs.hpp"
#include "ExtractRule.h"

int main(int argc, char** argv){  
	std::string sMessage;
	CmdArgs cmd;
	ExtractRule extraRule;

	if(false==cmd.parse(argc, argv, sMessage)){
		std::cout<<sMessage<<std::endl;
		return 1; 
	}

	extraRule.parse(cmd);
	
	return 0;
}	
























