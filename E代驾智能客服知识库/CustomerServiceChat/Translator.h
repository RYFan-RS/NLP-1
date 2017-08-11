#ifndef __AISMS__TRANSLATOR_H__
#define __AISMS__TRANSLATOR_H__

#include "Common/lib4/Utility.hpp"
#include "CustomerServiceChat/Common/Config.h"

#include "Common/lib4/DateString.hpp"
#include "Common/lib4/File.hpp"
#include "Common/lib4/Definations.hpp"
#include "Common/lib4/FileEncoder.hpp"
//#include "../MixParser/Parser.h"
#include "Common/KBConfig.h"
//#include "Debug.h"
#include <map>
#include <vector>
#include <iostream>

namespace aisms
{

class TransData{
public:
	bool read(const acl::Line& line);
	bool hasCondition(const std::string& sCondition) const;
public:
	std::vector<std::string> vCondition; // 词组数据的前置条件，比如属于某个城市
	std::vector<std::string> vWord; // 单词
	std::map<std::string, size_t> mAttribute_WordIdx; // 单词的别名，例如:XIY(Code) XIY是Code
};

class Translator // this class is in order to analyze the file named by Translation.dic; that generate a map which
                 // record content of the file. for example :　airport 北京 PEK
{
public:
	bool init(const acl::FileName& sXmlFile);
	
	void setMixParserPointer(mxpsr::Parser *pMixParser);
	
	const std::string& translate(const std::string& sDicName, const std::string& sIn) const;
	
	std::string translateWithParser(const std::string& sDicName, const std::string& sIn) const;
	
	const std::string& translateOrgName(const std::string& sDicName, const std::string& sIn) const;
	
	const std::string& translateWithCondition(const std::string& sDicName, const std::string& sIn, const std::string& sCondition) const;
	
	bool toFile(const std::string& sFilename) const;
private:
	bool readEncryptionFile(const std::string& sFilename);
	
	bool readNormalFile(const std::string& sFilename);
	
	bool initData(const acl::File& file);
	
private:
	std::map<std::string,std::multimap<std::string,size_t> > m_mDicIdx;
	std::vector<TransData> m_vTransData;
	mxpsr::Parser *m_pMixParser;
	std::string m_sEmpty;
};

}// namespace aisms

#endif //__AISMS__TRANSLATOR_H__
