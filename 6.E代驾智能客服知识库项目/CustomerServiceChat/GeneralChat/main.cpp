#include <iostream>
#include <string>
#include "KnowledgeBase.h"
#include "Common/lib4/IO.hpp"
#include "Common/lib4/File.hpp"
#include "Common/json/reader.h"
#include "Common/json/writer.h"
#include "Common/json/elements.h"
#ifndef PRINT_LOG
#define PRINT_LOG
#endif
#include "Common/lib5/WHL_LogInstance.hpp"

robosay::general::KnowledgeBase kb;

class CmdArgs {
public:
	CmdArgs() : m_bNaturalInput(false), m_bSemanticInput(false){}
	
public:
	std::string usage(){
		std::string sRet;
		
		sRet += "USAGE:\n";
		sRet += "    argument              parameter           intorduction\n";
		sRet += "    -h,--help                                 Print this message.\n";
		sRet += "    -v,--version                              Print version information and then exit.\n";
		sRet += "    -n,--natural                              natural language input";
		sRet += "    -s,--semantic                             semantic input";
		sRet += "    -i,--input            [file-name]         Read input from a txt file.\n";
		sRet += "    -o,--output           [file-name]         Write output to a file.\n";
		sRet += "\n";
		sRet += "EXAMPLES:\n";
		sRet += "    (自然语言自由输入)  ./main -n\n";
		sRet += "    (语义自由输入)      ./main -s\n";
		sRet += "    (自然语言文件输入)  ./main -n -i in.txt -o out.txt\n";
		sRet += "    (语义文件输入)      ./main -s -i in.txt -o out.txt\n";
		
		return sRet;
	}
	
	std::string version(){
		std::string sRet;
		sRet = "We don't have release version now!\n";
		return sRet;
	}
	
	bool parse(int argc, char** argv, std::string& sRet){
		std::vector<std::string> vArg;
		
		for(int i=1; i<argc; i++){
			vArg.push_back(argv[i]);
		}
		
		return parse(vArg,sRet);
	}
	
	bool parse(const std::vector<std::string>& vArg, std::string& sRet){
		if(vArg.empty()){
			sRet += "Error: Don't know what to do without argument!\n\n";
			sRet += usage();
			return false;
		}
		
		size_t uLast = vArg.size() - 1;
		//read arguments
		for(size_t i=0; i<vArg.size(); i++){
			if(vArg.at(i) == "-h" || vArg.at(i) == "--help"){
				sRet = usage();
				return false;
			}else if(vArg.at(i) == "-v" || vArg.at(i) == "--version"){
				sRet = version();
				return false;
			}else if(vArg.at(i) == "-n" || vArg.at(i) == "--natural"){
				// 自然语言方式输入标志
				m_bNaturalInput = true;
			}else if(vArg.at(i) == "-s" || vArg.at(i) == "--semantic"){
				// 语义方式输入标志
				m_bSemanticInput = true;
			}else if(vArg.at(i) == "-i" || vArg.at(i) == "--input"){
				if(i < uLast){
					m_sInFileName = vArg.at(++i);
				}else{
					return false;
				}
			}else if(vArg.at(i) == "-o" || vArg.at(i) == "--output"){
				if(i < uLast){
					m_sOutFileName = vArg.at(++i);
				}else{
					return false;
				}
			}else{
				sRet += "Error: Can't parse argument '" + vArg.at(i) + "'!\n\n";
				sRet += usage();
				return false;
			}
		}
		
		return true;
	}
	
	bool isNaturalInput() const{
		return m_bNaturalInput;
		std::cout<<"isNaturalInput!"<<std::endl;
	}
	
	bool isSemanticInput() const{
		return m_bSemanticInput;
	}
	
	const std::string& getInputFile() const{
		return m_sInFileName;
	}
	
	const std::string& getOutputFile() const{
		return m_sOutFileName;
	}
	
private:
	std::string m_sInFileName;
	std::string m_sOutFileName;
	bool        m_bNaturalInput;
	bool        m_bSemanticInput;
};

// 自由输入方式
int freeChat(bool bNeedAnalysis)
{
	robosay::base::VResult vPre;
	while(1) {
		acl::String sIn = acl::IO::STDIN("YouSay:");
		if(sIn == "q" || sIn == "quit")
			break;
		if(sIn == "RESET"){
			vPre.clear();
			continue;
		}
		robosay::base::VResult vResult;
		kb.execute(sIn, bNeedAnalysis, vResult,&vPre);			
		std::cout<<"RoboSay:-----------------------"<<std::endl;
		if(!vResult.empty()){
			std::cout<<"回答:"<<vResult.toStr()<<std::endl;
		}else{
			std::cout<<"错误:"<<vResult.sMessage<<std::endl;
		}
		if(vResult.sMessage.empty()){
			vPre.moveFrom(vResult);
		}
		std::cout<<std::endl;
	}

	return 0;
}

//文件输入方式
int fileChat(const CmdArgs& args, bool bNeedAnalysis)
{	
	acl::File in;
	acl::File out;
	
	if(args.getInputFile().empty()){
		std::cout<<"No input file defined!"<<std::endl;
		return false;
	}
	if(args.getOutputFile().empty()){
		std::cout<<"No ouput file defined!"<<std::endl;
		return false;
	}
	
	in.read(args.getInputFile());
	for(size_t i=0; i<in.size(); i++){
		robosay::base::VResult vResult;
		kb.execute(in[i], bNeedAnalysis,vResult);
		out.push_back("输入: ");
		out.push_back("      " + in[i]);
		out.push_back("输出: ");
		out.push_back("      " + vResult.toStr());
		out.push_back("\n\n");
		out.push_back("\n\n");
	}
	out.write(args.getOutputFile());
	
	return 0;
}


int main(int argc, char** argv)
{
	if(kb.init("../../../data/public/BasicModel/FAQ.xml") == false){
		ELOGGER->print(true, "main kb init ERROE");
		return 1;
	}
	
    CmdArgs args;
	std::string sMessage;
	bool bNeedAnalysis;
	
	if(args.parse(argc,argv,sMessage) == false){
		std::cout<<sMessage;
		return 2;
	}
	
	if(args.isNaturalInput()){
	// Natural language input mode
		bNeedAnalysis = true;
		if(args.getInputFile().empty()){    // Free chat mode
			freeChat(bNeedAnalysis);
			return 3;
		}else{                              // File chat mode
			fileChat(args, bNeedAnalysis);
			return 4;
		}
	}else if(args.isSemanticInput()){
	// Semantic language input mode
		bNeedAnalysis = false;
		if(args.getInputFile().empty()){
			freeChat(bNeedAnalysis);
			return 5;
		}else{
			fileChat(args, bNeedAnalysis);
			return 6;
		}
	}else{
		return 7;
	}
	
	return 0;
}
