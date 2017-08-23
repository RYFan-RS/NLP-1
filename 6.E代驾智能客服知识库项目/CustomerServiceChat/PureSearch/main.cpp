#include "Common/lib4/IO.hpp"

#include "SearchEngineEx.h"

/*sudo apt-get install libopencc-dev*/

int main(int argc, char** argv)
{
	robosay::base::SearchEngineEx searchEngine;

	if(argc!=2){
		std::cout<<"need one parameter. should ./main ./1.txt";
		return 0;
	}
	searchEngine.init(argv[1]);

	while(1) {
		std::vector<std::pair<std::string,std::string> > resultList;
		std::string errorInfo;
		
		acl::String sIn = acl::IO::STDIN("YouSay:");
		if(sIn == "q" || sIn == "quit")
			break;

        bool bRet = searchEngine.process(sIn,resultList,errorInfo);
        if(!bRet){
			std::cout<<"对话出现问题:"<<errorInfo<<std::endl;
        }else{

			std::cout<<"命中的question:"<<resultList[0].first <<std::endl;
			std::cout<<"命中的Answer:  "<<resultList[0].second<<std::endl;			
        }
	}

	
	return 0;
}
