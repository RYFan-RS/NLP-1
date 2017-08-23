#ifndef __AISMS__DYPSR__PARSER__H__
#define __AISMS__DYPSR__PARSER__H__
#include <list>
#include <iterator>
#include "lib4/ConfigFile.hpp"
#include "lib4/FileName.hpp"
#include "Dictionary.h"
#include "RuleOfParse.h"

namespace aisms
{
namespace dypsr
{

class Parser
{
public:
    // use config file to init multiple dictionaries
    bool init(const acl::FileName& configFileName, acl::String sDicDir="");
    // init one dictionary
    bool init(const std::string& sDicName, const std::vector<std::string>& vWord);
    // check the existence of a dictionary name
    bool hasDictionary(const std::string& sDicName) const;
    // parse an input string
	//增加参数3为默认参数，当bUpper为false时，不执行input.toUpper(); 用来规避类似/::g等表情拆分问题
    Result parse(acl::String input, const acl::String& sRule,bool bUpper=true) const;
    // get the words of specified dictionary
    std::vector<std::string> getWords(const std::string& sDicName) const;
    // get the words of all the dictionary
    std::vector<std::string> getWords() const;
private:
    void parse(Result& result, const Dictionary& dic) const;
private:
    std::map<std::string, Dictionary> m_mDicMap;
};

}//dypsr
}//namespace aisms

#endif //__AISMS__DYPSR__PARSER__H__

