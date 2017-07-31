#include "Parser.h"
#include "Common/lib4/File.hpp"
#include "Common/lib4/IO.hpp"
#include "Common/lib4/Debug.hpp"

void usage(char** argv){
	std::cout<<"USAGE:"<<std::endl;
	std::cout<<argv[0]<<std::endl;
	std::cout<<argv[0]<<"  [input-file] [output-file]"<<std::endl;
}

int parse(){
    aisms::dypsr::Parser diyParser;

    if(diyParser.init("dic.conf") == false) {
        return 1;
    }

	while(1){
        acl::String sIn = acl::IO::STDIN("INPUT:");
		if(sIn == "q" || sIn == "quit") break;
        //PersonalName;PlaceName;CommonDic
        aisms::dypsr::Result result = diyParser.parse(sIn,"all");
        //std::cout<<result.toStr()<<std::endl;
		std::cout<<"OUTPUT:"<<acl::debug::toStr(result.getWords())<<std::endl;
		std::cout<<std::endl;
    }
	
	return 0;
}

int parseWithFile(char** argv){
	acl::File inFile,outFile;
    aisms::dypsr::Parser diyParser;

    if(diyParser.init("../data/dic.conf") == false) {
        return 1;
    }

    if(inFile.read("in.txt") == false) {
        return 2;
    }

    if(inFile.empty()) {
        std::cout<<"No input data!"<<std::endl;
    }

    for(size_t i=0; i<inFile.size(); i++){
        if(inFile.empty()) continue;
        //PersonalName;PlaceName;CommonDic
        aisms::dypsr::Result result = diyParser.parse(inFile[i],"CommonDic");
        outFile.push_back("Result: "+result.toStr());
		std::cerr<<i<<std::endl;
    }

    outFile.write("out.txt");
	
	return 0;
}

int main(int argc, char** argv)
{
    if(argc == 1){
		return parse();
	}else if(argc == 3){
		return parseWithFile(argv);
	}else{
		usage(argv);
	}

    return 0;
}

