#ifndef WIN32
#include <getopt.h>
#include <sys/time.h>
#else
#pragma warning(disable: 4786)
#endif //WIN32
#include <fstream>
#include <sstream>
#include "Utility.h"
#include "FileManager.h"
#include "mean.h"
#include "Common/lib4/IO.hpp"
#include "Common/lib4/File.hpp"
#include "Common/lib4/Definations.hpp"
#include "SentenceParser/DynamicParser/Parser.h"

#ifdef WIN32
#define ERROR_LOG_FILE_PATH "MAE_ERROR_LOG.log"
#endif

using namespace std;
using namespace sae;

bool compile(const acl::FileName& semanticPath)
{
#ifndef WIN32
    //begin to record the parsing time
    struct timeval tBegin,tEnd;
    gettimeofday(&tBegin , NULL);
	std::cout<<"Begin to compile "<<semanticPath<<" ."<<std::endl;
#endif

    FileManager fileManager;

    //read meaning rule file
    int lineIndexID = -1;
    if(fileManager.readDataFile(semanticPath,lineIndexID) == false) {
#ifndef WIN32
        fileManager.dumpErrorData("",cout);
#else
        fstream ofs(ERROR_LOG_FILE_PATH,ios::out);
        if(ofs.is_open() == true) {
            fileManager.dumpErrorData("",ofs);
        }
        ofs.close();
#endif
        return 3;
    }

    //parsing and create execute data structure from the rules in the meaning rule file
    if(fileManager.parse() == false) {
#ifndef WIN32
        fileManager.dumpErrorData("",cout);
#else
        fstream ofs(ERROR_LOG_FILE_PATH,ios::out);
        if(ofs.is_open() == true) {
            fileManager.dumpErrorData("",ofs);
        }
        ofs.close();
#endif
        return 4;
    }
#ifndef WIN32
    else if( fileManager.hasErrorMessage() ) {
        fileManager.dumpErrorData("",cout);
    }
#endif

    //serialize execute data structure from memory to disk
    if(fileManager.makeStorage(semanticPath.getBaseName() + ".dat") == false) {
#ifndef WIN32
        fileManager.dumpErrorData("",cout);
#else
        fstream ofs(ERROR_LOG_FILE_PATH,ios::out);
        if(ofs.is_open() == true) {
            fileManager.dumpErrorData("",ofs);
        }
        ofs.close();
#endif
        return 5;
    }

#ifndef WIN32
    //finish to record the parsing time
    gettimeofday(&tEnd , NULL);

    //fileManager.dumpStorage("DUMP DATA STORAGE:");

    //print out the parsing time
    std::cout<<"\033[1;32m"<<"Compile "<<semanticPath<<" successfully finished !"<<"\033[0m"<<std::endl;
    std::cout<<"Parsed "<<lineIndexID<<" lines, ";
    std::cout<<"cost "<<(tEnd.tv_sec-tBegin.tv_sec)*1000+(tEnd.tv_usec-tBegin.tv_usec)/1000<<" milliseconds.\n";
#endif
	
	return true;
}

int main(int argc, char** argv)
{
	acl::FileName sSmtcPath;
	if(argc < 2){
		std::string sRet;
		sRet += "USAGE:\n";
		sRet += "    ./SemanticCheck semantic.txt\n";
		std::cout<<sRet<<std::endl;
		sSmtcPath = "semantic.txt";
	}else{
		sSmtcPath = argv[1];
	}
	if(access(sSmtcPath.c_str(), 0)){
		std::cout<<"File ["<<sSmtcPath<<"] doesn't exist!"<<std::endl<<std::endl;
	}else{
		bool bRet = compile(sSmtcPath);
		if(!bRet){
			
		}else{
			
		}
	}
	
	return 0;
}
