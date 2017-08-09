#ifndef __EXTRACT_RULE__
#define __EXTRACT_RULE__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "CmdArgs.hpp"
#include "lib4/Algorithm.hpp"
#include "lib4/Directory.hpp"
#include "cppjieba/Jieba.hpp"
#include <assert.h>
#include <algorithm>
#include <vector>
#include <iostream>

class ExtractRule
{

public:
	ExtractRule():cppjiebaParser(NULL){

	}
	~ExtractRule(){
		delete (reinterpret_cast<cppjieba::Jieba*>(cppjiebaParser));
		cppjiebaParser = NULL;
	}
	bool CompareRules( std::vector<std::set<std::string> > &set1, std::vector<std::set<std::string> > &set2);
	void setRuleClass(int Class1,int Class2,std::map<int,int> &mClassRecord,std::vector<std::vector<int> > &vSameRule);	
	void getSameRule(std::map<int,std::vector<std::set<std::string> > > &mAllMap,std::vector<std::vector<int> > &vSameRule);
	void showSameRule(CmdArgs & cmd,std::vector<std::vector<int> > &vSameRule,std::string fileName);
	bool readFile(CmdArgs & cmd,std::string fileName);

	void initStaticParser(const std::string& userDefineWordFilePath = "");	
    void parse(CmdArgs &cmd);

private:
	std::map<int,std::string> mOriginRuleIndex;//每一句规则，对应一个编号
	void *cppjiebaParser;
};

#endif //__EXTRACT_RULE__

