#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "CommonParser.h"
#include "TagManager.h"
#include "Utility.h"

namespace sae
{

TagManager::TagManager()
{
    setDatabaseType(TAG_DATABASE);
}

bool TagManager::parse(const LINE& line, ErrorData& errData)
{
    std::string newLine;
    if(line.m_line[0] == '(') {
        newLine = line.m_line.substr(1,line.m_line.length()-2);
    } else {
        newLine = line.m_line;
    }

    std::vector<std::string> vElement = CommonParser::divideElements(newLine,TagDefineRuleElementsNumber,errData);
    if(vElement.empty()) {
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    if(Utility::isPureNumber(vElement[3]) == false) {
        errData.m_errMsg = Message::msg012();
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    int tagNumber = atoi(vElement[3].c_str());
    int returnNumber = find(vElement[1]);
    if(returnNumber != m_failCode) {
		errData.m_errMsg = Message::msg013(vElement[1]);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    if(mc_lineMap.find(tagNumber) != mc_lineMap.end()) {
        errData.m_errMsg = Message::msg014(vElement[3]);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    insert(std::make_pair(vElement[1],tagNumber));
    mc_lineMap.insert(std::make_pair(tagNumber,line.m_lineNum));

    //record the attributes
    std::vector<std::string> vAttributes = Utility::divide(vElement[2],"/");
    for(size_t i=0; i<vAttributes.size(); i++) {
        m_attributeMap.insert(std::make_pair(vAttributes[i],tagNumber));
    }
//	std::cerr<<toStr();
    return true;
}

std::string TagManager::toStr() const{
    std::stringstream sRet;
    const std::multimap<std::string,int>::const_iterator last = m_attributeMap.end();
    std::multimap<std::string, int>::const_iterator itr = m_attributeMap.begin();

    for(; last != itr; ++itr){
        sRet<<"group:["<<itr->first<<"]\ttag:["<<itr->second<<"]"<<std::endl;
    }
    return sRet.str();
}

std::map<std::string,int> TagManager::getTagMapWithAttribute(const std::string attribute) const
{
//    std::cerr<<toStr();
    std::map<std::string,int> m;
    std::pair<std::multimap<std::string,int>::const_iterator,std::multimap<std::string,int>::const_iterator> pIte;
    std::multimap<std::string,int>::const_iterator ite;
    std::string sTag;

    pIte = m_attributeMap.equal_range(attribute);
    for(ite = pIte.first; ite != pIte.second; ++ite) {
        sTag = find(ite->second);
        m.insert(std::make_pair(sTag,ite->second));
    }

    return m;
}

std::set<int> TagManager::getTagSetWithAttribute(const std::string attribute)
{
    std::set<int> s;
    std::pair<std::multimap<std::string,int>::iterator,std::multimap<std::string,int>::iterator> pIte;
    std::string sTag;

    pIte = m_attributeMap.equal_range(attribute);
    for(std::multimap<std::string,int>::iterator ite = pIte.first; ite != pIte.second; ++ite) {
        s.insert(ite->second);
    }

    return s;
}

}//namespace sae
