



#include "Parser.h"
#include "Common/lib4/Time.hpp"

namespace aisms
{
namespace dypsr
{

Parser::~Parser(){
	std::map<std::string, Dictionary*>::iterator iter;
	for(iter=m_mDicMap.begin(); iter!=m_mDicMap.end(); ++iter){
		delete iter->second;
		iter->second = NULL;
	}
	m_mDicMap.clear();
}

bool Parser::init(const acl::FileName& configFileName, acl::String sDicDir){
    acl::ConfigFile config;
	acl::FileName sFileFullPath;

    //read ConfigFile
    if(config.read(configFileName) == false) {
		PRINT("WARNING: Init dynamic parser without config file.")
    }

    if(sDicDir.empty()){
        sDicDir = configFileName.getPath();
    }
	
	acl::Time timer;
	timer.start("PARSE");
    //init each dictionary listed in the config file.
    const std::map<acl::String,acl::String>& mParamMap = config.getParamMap();
    for(std::map<acl::String,acl::String>::const_iterator iter=mParamMap.begin(); iter!=mParamMap.end(); ++iter) {
        Dictionary* dic = new Dictionary();
#ifdef USE_ENCODE_DECODE
		sFileFullPath = sDicDir +"/"+iter->second;
		if(dic->initWithEncryption(sFileFullPath.getBaseName() + ".dat") == false) {
#else
		sFileFullPath = sDicDir +"/"+iter->second;
       
        if(dic->init(sFileFullPath.getBaseName() + ".dic") == false) {
#endif
			PRINT("DynmaicParser init with file \""<<sFileFullPath<<"\" failed!")
            return false;
        }
		//此处使用指针,避免当数据量足够大的时候，过分消耗内存
        m_mDicMap.insert(std::make_pair(iter->first,dic));
    }
	timer.end("PARSE");
	PE(timer.toStr("PARSE"))
	
    return true;
}

bool Parser::init(const std::string& sDicName, const std::vector<std::string>& vWord)
{
    std::map<std::string, Dictionary*>::iterator iter = m_mDicMap.find(sDicName);

    if( iter == m_mDicMap.end()) {
        Dictionary* dic = new Dictionary();
		if(dic->init(vWord) == false) {
			return false;
		}
		m_mDicMap.insert(std::make_pair(sDicName,dic));
    }else{
        Dictionary* dic = iter->second;
		if(dic->init(vWord) == false) {
			return false;
		}
    }

    return true;
}

bool Parser::hasDictionary(const std::string& sDicName) const
{
    return m_mDicMap.find(sDicName) == m_mDicMap.end() ? false : true;
}

Result Parser::parse(acl::String input, const acl::String& sRule,bool bUpper) const
{
	if(bUpper)
	{
		input.toUpper();
	}
	//输入进行预处理
    Result result(input);
    RuleOfParse rop;
    std::map<std::string, Dictionary*>::const_iterator iter;

    if(rop.parse(sRule) == false) {
#ifdef _DYNAMIC_PARSER_DEBUG
		ssDebug<<"The format of parse rule \"sRule\" error";
		acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL0);
#endif
        return result;
    }
	
	//acl::Time timer;
	//timer.start("FIND");
	
    for(size_t i=0; i<rop.size(); i++) {
        iter = m_mDicMap.find(rop.at(i));
        if(iter == m_mDicMap.end()) {
#ifdef _DYNAMIC_PARSER_DEBUG
			ssDebug<<"WARNING: Can't find dictionary \""<<rop.at(i)<<"\" .";
			acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL0);
#endif
            continue;
        }

#ifdef _DYNAMIC_PARSER_DEBUG
		ssDebug<<"Use dictionary \""<<rop.at(i)<<"\" to parse "<<acl::debug::toStr(result);
		acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL0);
		ssDebug<<"Dictionary words: "<<acl::debug::toStr(iter->second.getWords());
		acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL0);
#endif
        parse(result,iter->second);
#ifdef _DYNAMIC_PARSER_DEBUG
		ssDebug<<"Parse result is "<<result.toStr();
		acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL0);
		ssDebug<<"Parsed words are "<<acl::debug::toStr(result.getWords());
		acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL0);
#endif
    }
	
	//timer.end("FIND");
	//PE(timer.toStr("FIND"))
	
    return result;
}

void Parser::parse(Result& result, const Dictionary* dic) const
{
    for(size_t i=0; i<result.size(); i++) {
        Range range = dic->find(result,i);
        if(!range.isNull()) {
			result.add(range);
#ifdef _DYNAMIC_PARSER_DEBUG
			ssDebug<<"Result range is "<<range.toStr();
			acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL1);
			ssDebug<<"Parse result updates to "<<result.toStr();
			acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL1);
#endif
			//从本次已经查找到的词语的下一个开始继续分词
            i = range.second;
        } else {
			//将单个单词作为一个分词
            range.setValue(i,i);
            result.add(range);
        }
    }
}

std::vector<std::string> Parser::getWords(const std::string& sDicName) const
{
	std::vector<std::string> vWord;
	std::map<std::string, Dictionary*>::const_iterator iter = m_mDicMap.find(sDicName);
	
	if(iter != m_mDicMap.end()){
		vWord = iter->second->getWords();
	}
	
	return vWord;
}

std::vector<std::string> Parser::getWords() const
{
	std::vector<std::string> vWord;
	std::map<std::string, Dictionary*>::const_iterator iter;
	
	for(iter=m_mDicMap.begin(); iter!=m_mDicMap.end(); ++iter){
		std::vector<std::string> v = iter->second->getWords();
		for(size_t i=0; i<v.size(); i++){
			vWord.push_back(iter->first + "\t" + v.at(i));
		}
	}
	
	return vWord;
}

}//dypsr
}//namespace aisms

