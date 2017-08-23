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
#include "Common/lib4/Debug.hpp"

#include "SentenceParser/DynamicParser/Parser.h"

#ifdef WIN32
#define ERROR_LOG_FILE_PATH "MAE_ERROR_LOG.log"
#endif

using namespace std;
using namespace sae;

class CmdArgs {
public:
	CmdArgs() : m_bIsCompile(false), m_bIsExecute(false), m_bIsUnitTest(false), m_bIsWordList(false){}
	
public:
	std::string usage(){
		std::string sRet;
		
		sRet += "USAGE:\n";
		sRet += "    argument              parameter           intorduction\n";
		sRet += "    -h,--help                                 Print this message.\n";
		sRet += "    -v,--version                              Print version information and then exit.\n";
		sRet += "    -c,--compile          [file-name]         Compile a rule file.\n";
		sRet += "    -e,--execute          [file-name]         Execute a rule data file.\n";
		sRet += "    -u,--unittest         [file-name]         Do unit test to a rule data file.\n";
		sRet += "    -w,--wordlist         [file-name]         Get the word list of the rule data file.\n";
		sRet += "    -i,--input            [file-name]         Read input from a txt file.\n";
		sRet += "    -o,--output           [file-name]         Write output to a file.\n";
		sRet += "    -r,--rule             [RuleIndexRange]    match Rule Index Range.\n";
		
		sRet += "\n";
		sRet += "EXAMPLES:\n";
		sRet += "    ./main -c semantic.txt -o semantic.dat\n";
		sRet += "    ./main -e semantic.dat -i in.txt -o out.txt\n";
		sRet += "    ./main -u semantic.dat -i in.txt -o out.txt\n";
		sRet += "    ./main -w semantic.dat -o out.txt\n";
		sRet += "    ./main -s semantic.dat -i in.txt -o out.txt\n";	
		sRet += "    ./main -e semantic.dat -i in.txt -o out.txt -r 1-1000:0; or AllRules\n";		
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
			}else if(vArg.at(i) == "-c" || vArg.at(i) == "--compile"){
				m_bIsCompile = true;
				if(i < uLast){
					m_sRuleFileName = vArg.at(++i);
				}else{
					return false;
				}
			}else if(vArg.at(i) == "-e" || vArg.at(i) == "--execute"){
				m_bIsExecute = true;
				if(i < uLast){
					m_sExecuteFileName = vArg.at(++i);
				}else{
					return false;
				}
			}else if(vArg.at(i) == "-u" || vArg.at(i) == "--unittest"){
				m_bIsUnitTest = true;
				if(i < uLast){
					m_sExecuteFileName = vArg.at(++i);
				}else{
					return false;
				}
			}else if(vArg.at(i) == "-w" || vArg.at(i) == "--wordlist"){
				m_bIsWordList = true;
				if(i < uLast){
					m_sExecuteFileName = vArg.at(++i);
				}else{
					return false;
				}
			}else if(vArg.at(i) == "-r" || vArg.at(i) == "--rule"){
				m_bIsRuleRange = true;
				if(i < uLast){
					m_sRuleRange = vArg.at(++i);
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
	
	bool isCompile() const{
		return m_bIsCompile;
	}
	
	bool isExecute() const{
		return m_bIsExecute;
	}
	
	bool isUnitTest() const{
		return m_bIsUnitTest;
	}

	bool isRuleRange() const{
		return m_bIsRuleRange;
	}
	
	bool isWordList() const{
		return m_bIsWordList;
	}
	
	const std::string& getInputFile() const{
		return m_sInFileName;
	}
	
	const std::string& getOutputFile() const{
		return m_sOutFileName;
	}
	
	const std::string& getExecuteFile() const{
		return m_sExecuteFileName;
	}
	
	const std::string& getRuleFile() const{
		return m_sRuleFileName;
	}

	const std::string& getRuleRange() const{
		return m_sRuleRange;
	}

	
private:
	std::string m_sInFileName;
	std::string m_sOutFileName;
	std::string m_sExecuteFileName;
	std::string m_sRuleFileName;
	std::string m_sRuleRange;
	bool        m_bIsCompile;
	bool        m_bIsExecute;
	bool        m_bIsUnitTest;
	bool        m_bIsWordList;
	bool        m_bIsRuleRange;
};

bool unittest(const CmdArgs& args)
{
	if(args.getInputFile().empty()){
		std::cout<<"Error: Can't open input file ["<<args.getInputFile()<<"] !"<<std::endl;
		return false;
	}
	
	ContextDictionary* mae = new ContextDictionary();
    if(mae->init(args.getExecuteFile()) != 0) {
        std::cout<<"Error: Can't initalize Semantic Engine with file ["<<args.getExecuteFile()<<"] !"<<std::endl;
		delete mae;
        return false;
    }

    aisms::dypsr::Parser parser;
    if(parser.init("DynamicDic",mae->getData().getConstWordManager().getWords()) == false) {
        std::cout<<"Error: Initalize dynamic parser failed !"<<std::endl;
		delete mae;		
        return false;
    }
	
	acl::File inFile,outFile;
	float nTotalInput=0,nFailedInput=0;
    inFile.read(args.getInputFile());
    for(size_t i=0; i<inFile.size(); i++) {
        if(inFile[i].empty()) continue;
        std::vector<std::string> v = inFile[i].separate("\t");
		if(v.size() < 2){
			std::cout<<"Error: Standard file format error at line "<<i+1<<" !"<<std::endl;
			std::cout<<"    +-Standard file format--+"<<std::endl;
			std::cout<<"    |SEMANTIC\\tINPUT SENTENCE1\\tINPUT SENTENCE2\\t......|"<<std::endl;
			delete mae;
			return false;
		}
		
		std::string sResult = "标签:"+v[0]+"\n不匹配的结果为:\n";
        //例如,label1&label2,代表必须满足两个标签,此时vSemantic大小为2,当标签为NULL时,大小为1
		std::vector<std::string> vSemantic = acl::Utility::separate(v[0],"&");
		
		nTotalInput += v.size() - 1;

        //解析标签，可能存在多个
        //label1&label1代表必须同时满足这2个标签,label1&!label2,代表满足label1但是不能满足label2
        //NULL代表任何标签都不满足

        std::vector<std::string> vPositiveSemantic;
        std::vector<std::string> vNegtiveSemantic;  
        for(size_t j=0; j<vSemantic.size(); j++) {
                //如果是中文的"!"会无法识别，请注意
                if(acl::Utility::isBeginWith(vSemantic[j],"!")){
                    //代表这个规则是不能中的
                    vNegtiveSemantic.push_back(vSemantic[j].substr(1));
                }
                else {
                    //应该中的
                    vPositiveSemantic.push_back(vSemantic[j]);
                }
            }
                                       
		for(size_t iIn=1; iIn<v.size(); iIn++){
			if(v[iIn].empty()) continue;
            
			std::vector<int> vPositiveFlags(vPositiveSemantic.size(),0);
            std::vector<int> vNegtiveFlags(vNegtiveSemantic.size(),0);
            
			aisms::dypsr::Result result = parser.parse(v[iIn],"DynamicDic");
			
			std::cout<<"\n"<<v[iIn]<<":分词结果为:"<<acl::debug::toStr(result.getWords())<<std::endl;
			MeanWords words(result.getWords(),&mae->getExcutableData().getDataManager());
            //每一句话的匹配结果
			VMeanResult vResult = mae->analyze("", words, mae->getExecuteMethod("NORMAL_ANALYSIS"));
			
			if(vResult.empty()){
				bool bAllMatched = 	false;
				if(1==vSemantic.size()){
					std::string strSemantic = acl::Utility::toUpper(vSemantic[0]);
                    //当结果一条都没有匹配并且只有一个NULL的label,其实就是匹配
					if(strSemantic == "NULL"){
						bAllMatched = true;			
					}
				}	
                if(false == bAllMatched){
					sResult += "\t"+v[iIn];				
					nFailedInput++;
            	}
			}
            else{
			    //mTagContent:label和sentence或者label和word
				std::multimap<std::string,std::string> mTagContent = vResult.getTagContentMap(words);
				bool bAllMatched = true;
				bool handle = false;
				std::ostringstream oss;

				vResult.dump(oss);

				if(1==vSemantic.size()){
					std::string strSemantic = acl::Utility::toUpper(vSemantic[0]);
                    //当结果存在匹配并且只有一个NULL的label,其实就是不匹配
					if(strSemantic == "NULL"){
						handle = true;
						bAllMatched = false;
					}
				}	
				if(!handle){
					std::multimap<std::string,std::string>::const_iterator  itTagMap = mTagContent.begin();

					for(;itTagMap!=mTagContent.end();itTagMap++){

                        //std::cout<<"####"<<v[iIn]<<"####"<<itTagMap->first<<":"<<itTagMap->second<<"####"<<oss.str()<<std::endl;
                    
                        //比较正例
						for(size_t j=0; j<vPositiveSemantic.size(); j++){                             
							if(acl::Utility::toUpper((const std::string)vPositiveSemantic[j]) == acl::Utility::toUpper(itTagMap->first)){
                                //如果某一个标签与某一个匹配结果的标签对应，就认为这条标签被匹配上了
                                vPositiveFlags[j] = 1;
							}
						}
                        //比较反例
						for(size_t j=0; j<vNegtiveSemantic.size(); j++){                             
							if(acl::Utility::toUpper((const std::string)vNegtiveSemantic[j]) == acl::Utility::toUpper(itTagMap->first)){
                                //如果某一个标签与某一个匹配结果的标签对应，就认为这条标签被匹配上了
                                vNegtiveFlags[j] = 1;
							}
						} 
                        
					}
				}

                if(vPositiveFlags.end() != find(vPositiveFlags.begin(),vPositiveFlags.end(),0)){
                        //存在没有中的
                        bAllMatched = false;
                    }
                else if(vNegtiveFlags.end() != find(vNegtiveFlags.begin(),vNegtiveFlags.end(),1)){
                        //不应该中的但是中了
                        bAllMatched = false;
                    }
				if(bAllMatched == false){
					sResult += v[iIn]+ oss.str()+"\n";
					nFailedInput++;
				}
			}
		}
		
		if(sResult != v[0]){
			outFile.push_back(sResult);
		}
    }
	
	std::stringstream ss;
	ss << "输入文总数：" << nTotalInput << "，失败回答文总数：" << nFailedInput << "，失败率：" << nFailedInput*100/nTotalInput<<"% 。";
	outFile.push_back("");
	outFile.push_back(ss.str());
	
	if(args.getOutputFile().empty()){
		std::copy(outFile.begin(),outFile.end(),ostream_iterator<std::string>(std::cout,"\n"));
	}else{
		outFile.write(args.getOutputFile());
	}

    std::cout<<"Unit test successfully finished!"<<std::endl;
	delete mae;			
	return true;
}

bool executeWithRuleRange(const CmdArgs& args){
	std::string executeMethod;
	bool bUseDefault = false;
	if(sae::Utility::toUpper("AllRules")== sae::Utility::toUpper(args.getRuleRange())){
		bUseDefault = true;
	}

	if(args.getInputFile().empty() || args.getOutputFile().empty()){
		std::cout<<"No input or Output file defined, error. return."<<std::endl;
		return false;
	}
	
    ContextDictionary* mae = new ContextDictionary();
    if(mae->init(args.getExecuteFile()) != 0) {
        std::cout<<"Error: Can't initalize Semantic Engine with file ["<<args.getExecuteFile()<<"] !"<<std::endl;
		delete mae;			
        return false;
    }

    aisms::dypsr::Parser parser;
    if(parser.init("DynamicDic",mae->getData().getConstWordManager().getWords()) == false) {
        std::cout<<"Error: Initalize dynamic parser failed !"<<std::endl;
		delete mae;			
        return false;
    }
	
    acl::File inFile,outFile;
    if( !args.getInputFile().empty() && inFile.read(args.getInputFile()) ){
		for(size_t i=0; i<inFile.size(); i++) {
			if(inFile[i].empty()) continue;
			std::string input = inFile[i];
			aisms::dypsr::Result result = parser.parse(input,"DynamicDic");
			MeanWords words(result.getWords(),&mae->getExcutableData().getDataManager());
			outFile.push_back("Input Words: " + words.dump2Str());
			outFile.push_back("------------------------------------------");

			VMeanResult meanResult;
			if(bUseDefault)
				meanResult = mae->analyze("", words, mae->getExecuteMethod("NORMAL_ANALYSIS"));
			else{
				ExecuteManager executeManager;
				LINE line;
				line.m_lineNum = 1;
				line.m_line = "(#EXECUTION\tNORMAL_ANALYSIS\t"+args.getRuleRange()+")";
				ErrorData errData;
				if(executeManager.parse(line,&mae->getExcutableData().getDataManager(),errData)){
					const std::vector<ExecutePhase>& vecExecutePhase = executeManager.getExecuteMethod("NORMAL_ANALYSIS");
					if(vecExecutePhase.size()!=0){
						meanResult = mae->analyze("", words, executeManager.getExecuteMethod("NORMAL_ANALYSIS"));	
					}
					else
						std::cout<<"single sentence no execution Method."<<line.m_line<<std::endl;
				}
				else{
					std::cout<<"single sentence execution Method is error."<<line.m_line<<std::endl;
				}
			}
				

			std::stringstream ss;
			if(meanResult.size()!=0){
				meanResult.dump(ss);
				outFile.push_back(ss.str());
				outFile.push_back("Output Words: " + words.dump2Str());
				outFile.push_back("match successfully");
    			std::cout<<"Execution successfully finished!"<<std::endl;	
			}
			else
    			std::cout<<"match failed!"<<std::endl;			
			
			break;
		}
	}
	
	outFile.write(args.getOutputFile());
	delete mae;		
	return true;
}

bool execute(const CmdArgs& args)
{
	if(args.getInputFile().empty()){
		std::cout<<"No input file defined, switch to free test mode!"<<std::endl;
	}
	
    ContextDictionary* mae = new ContextDictionary();
    if(mae->init(args.getExecuteFile()) != 0) {
        std::cout<<"Error: Can't initalize Semantic Engine with file ["<<args.getExecuteFile()<<"] !"<<std::endl;
		delete mae;
        return false;
    }

    aisms::dypsr::Parser parser;
    if(parser.init("DynamicDic",mae->getData().getConstWordManager().getWords()) == false) {
        std::cout<<"Error: Initalize dynamic parser failed !"<<std::endl;
		delete mae;
        return false;
    }
	
    acl::File inFile,outFile;
    if( !args.getInputFile().empty() && inFile.read(args.getInputFile()) ){
		for(size_t i=0; i<inFile.size(); i++) {
			if(inFile[i].empty()) continue;
			std::string input = inFile[i];
			aisms::dypsr::Result result = parser.parse(input,"DynamicDic");
			MeanWords words(result.getWords(),&mae->getExcutableData().getDataManager());
			outFile.push_back("Input Words: " + words.dump2Str());
			outFile.push_back("------------------------------------------");
			VMeanResult meanResult = mae->analyze("", words, mae->getExecuteMethod("NORMAL_ANALYSIS"));

			std::stringstream ss;
			meanResult.dump(ss);
			outFile.push_back(ss.str());
			outFile.push_back("Output Words: " + words.dump2Str());
			outFile.push_back("======================================================");
		}
	}else{
		while(1){
			acl::String sInput = acl::IO::STDIN();
			if(sInput == "q" || sInput == "quit") break;
			
			aisms::dypsr::Result result = parser.parse(sInput,"DynamicDic");
			MeanWords words(result.getWords(),&mae->getExcutableData().getDataManager());
			std::cout<<"Input Words: " << words.dump2Str()<<std::endl;
			std::cout<<"------------------------------------------"<<std::endl;
			VMeanResult meanResult = mae->analyze("", words, mae->getExecuteMethod("NORMAL_ANALYSIS"));

			std::stringstream ss;
			meanResult.dump(ss);
			std::cout<<ss.str()<<std::endl;
			std::cout<<"Output Words: " << words.dump2Str()<<std::endl;
			std::cout<<"======================================================"<<std::endl;
		}
	}
	
	if(args.getOutputFile().empty()){
		std::copy(outFile.begin(),outFile.end(),ostream_iterator<std::string>(std::cout,"\n"));
	}else{
		outFile.write(args.getOutputFile());
	}

    std::cout<<"Execution successfully finished!"<<std::endl;
	delete mae;

    return true;
}

bool wordlist(const CmdArgs& args)
{
	acl::File outFile;
    ContextDictionary* se = new ContextDictionary();
	
    if(se->init(args.getExecuteFile()) != 0) {
        std::cout<<"Error: Can't initalize Semantic Engine with file ["<<args.getExecuteFile()<<"] !"<<std::endl;
		delete se;
        return false;
    }
	
	std::vector<std::string> vWord = se->getWordsByTag("");
	
	std::copy(vWord.begin(),vWord.end(),back_inserter(outFile));
	
	if(args.getOutputFile().empty()){
		std::copy(outFile.begin(),outFile.end(),ostream_iterator<std::string>(std::cout,"\n"));
	}else{
		outFile.write(args.getOutputFile());
	}
	
    std::cout<<"Word list successfully finished!"<<std::endl;

	delete se;
    return true;
}

bool compile(const CmdArgs& args)
{
	if(args.getOutputFile().empty()){
		std::cout<<"Error: Can't open output file ["<<args.getOutputFile()<<"] !"<<std::endl;
		return false;
	}
#ifndef WIN32
    //begin to record the parsing time
    struct timeval tBegin,tEnd;
    gettimeofday(&tBegin , NULL);
	std::cout<<"Begin to compile "<<args.getRuleFile()<<" ."<<std::endl;
#endif

    FileManager fileManager;

    //read meaning rule file
    int lineIndexID = -1;
    if(fileManager.readDataFile(args.getRuleFile(),lineIndexID) == false) {
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
    if(fileManager.makeStorage(args.getOutputFile()) == false) {
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
    std::cout<<"\033[1;32m"<<"Compile "<<args.getRuleFile()<<" successfully finished !"<<"\033[0m"<<std::endl;
    std::cout<<"Parsed "<<lineIndexID<<" lines, ";
    std::cout<<"cost "<<(tEnd.tv_sec-tBegin.tv_sec)*1000+(tEnd.tv_usec-tBegin.tv_usec)/1000<<" milliseconds.\n";
#endif
	return true;
}

int main(int argc, char** argv)
{
    CmdArgs args;
	std::string sMessage;
	
	if(args.parse(argc,argv,sMessage) == false){
		std::cout<<sMessage;
		return 1;
	}
	
	if(args.isCompile()){
		if(compile(args) == false){
			return 2;
		}
	}else if(args.isExecute()&& args.isRuleRange()){
		if(executeWithRuleRange(args) == false){
			return 3;
		}	
	}else if(args.isExecute()){
		if(execute(args) == false){
			return 3;
		}
	}else if(args.isUnitTest()){
		if(unittest(args) == false){
			return 4;
		}
	}else if(args.isWordList()){
		if(wordlist(args) == false){
			return 4;
		}
	}else{
		return 5;
	}
	
	return 0;
}
