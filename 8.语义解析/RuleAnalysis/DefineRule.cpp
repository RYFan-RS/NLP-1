#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "CommonParser.h"
#include "Utility.h"
#include "DefineRule.h"
#include <algorithm>

namespace sae
{

void DefineRule::makeWarningIfThereAreFunctionMarks(const std::string& value, ErrorData& errData) const
{
    if(value.find("/") != std::string::npos) {
        errData.m_warnMsg = Message::msg073();
        errData.warnTrace(__FILE__,__LINE__-1);
        return;
    }
    if(value.find("|") != std::string::npos) {
        errData.m_warnMsg = Message::msg074();
        errData.warnTrace(__FILE__,__LINE__-1);
        return;
    }
}
bool DefineRule::preParse(const std::string& value,std::vector<std::string> &vRelatedDefine, ErrorData& errData) const
{
	std::vector<std::string> val = Utility::divide(value,",");

	int ret = Utility::hasEmptyCell(val);
	if( ret > -1 ) {
		errData.m_errMsg = Message::msg071(ret);
		errData.errorTrace(__FILE__,__LINE__-1);
		return false;
	}

	std::sort(val.begin(),val.end());
	std::reverse(val.begin(),val.end());

	for(size_t i=0; i<val.size(); i++) {
		std::string keyWord,functionMark,basicWordMark;
		if(CommonParser::isSynonymExtend(val[i]) == true) { //extend the synonym type
			if(CommonParser::getSynonymExtendKeyword(val[i],functionMark,keyWord,basicWordMark) == false) {
				errData.m_errMsg = Message::msg075(val[i]);
				errData.errorTrace(__FILE__,__LINE__-1);
				return false;
			} else {
				vRelatedDefine.push_back(keyWord);
			}
		}
	}

	return true;
}

bool DefineRule::parse(const std::string& value, DefineManager& defineManager, ErrorData& errData)
{
    std::vector<std::string> val = Utility::divide(value,",");

    int ret = Utility::hasEmptyCell(val);
    if( ret > -1 ) {
        errData.m_errMsg = Message::msg071(ret);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    std::sort(val.begin(),val.end());
    std::reverse(val.begin(),val.end());

    for(size_t i=0; i<val.size(); i++) {
        std::string keyWord,functionMark,basicWordMark;
        if(CommonParser::isSynonymExtend(val[i]) == true) { //extend the synonym type
            if(CommonParser::getSynonymExtendKeyword(val[i],functionMark,keyWord,basicWordMark) == false) {
                errData.m_errMsg = Message::msg075(val[i]);
                errData.errorTrace(__FILE__,__LINE__-1);
                return false;
            } else {
                const DefineRule& rule = defineManager.find(keyWord);
                if( !(rule.getType() & BASIC_DEFINITION) ) {
                    errData.m_errMsg = Message::msg076(keyWord);
                    errData.errorTrace(__FILE__,__LINE__-1);
                    return false;
                }
                const std::vector<std::string>& vWord = rule.getVal();
                if(vWord.empty()) {
                    errData.m_errMsg += Message::msg077(keyWord);
                    errData.errorTrace(__FILE__,__LINE__-1);
                    return false;
                } else {
                    for(size_t j=0; j<vWord.size(); j++) {
                        m_val.push_back(vWord[j]);
                    }
                }
            }
        } else {
            m_val.push_back(val[i]);
        }
    }
    
    //std::sort(m_val.begin(),m_val.end());
    //std::reverse(m_val.begin(),m_val.end());
    
    setType(BASIC_DEFINITION);

    makeWarningIfThereAreFunctionMarks(value,errData);

    return true;
}

bool DefineRule::parseConditionalDefine(const std::string& value, DefineManager& defineManager, ErrorData& errData)
{
    m_val.push_back(value);
    setType(CONDITIONAL_DEFINITION);
    return true;
}

bool DefineRule::parseKeyDefine(const std::string& value, DefineManager& defineManager, ErrorData& errData)
{
    m_val.push_back(value);
    setType(KEY_DEFINITION);
    return true;
}

bool DefineManager::isUniqKey(const std::string& key) const
{
    return m_synonymMap.find(key) == m_synonymMap.end() ? true : false;
}

std::string DefineManager::getRuleFromLine(const std::string& sLine) const
{
    size_t pos = sLine.find("\t");
    if(sLine[0] == '(') {
        return sLine.substr(pos+1,sLine.length()-pos-2);
    } else {
        return sLine.substr(pos+1);
    }
}

bool DefineManager::divideKeyAndValue(std::string& rule, std::string& key, std::string& value, ErrorData& errData) const
{
    Utility::removeSpaceNear(rule,"=");
    std::vector<std::string> v = Utility::divide(rule,"=");
    if(v.size() != 2) {
        errData.m_errMsg = Message::msg078();
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }
    int ret = Utility::hasEmptyCell(v);
    if( ret > -1 ) {
        errData.m_errMsg = Message::msg071(ret);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }
    //std::map<std::string,DefineRule>::iterator ite;
    //ite = m_synonymMap.find(v[0])
    if(isUniqKey(v[0]) == false) {
        errData.m_errMsg = Message::msg079(v[0]);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    key = v[0];
    value = v[1];

    return true;
}

bool DefineManager::preParse(const std::string& sDefine,ErrorData& errData,std::string &sName,std::vector<std::string> &vRelatedDefine ) const
{
	if( CommonParser::checkElementsNumber(sDefine,DefineRuleElementsNumber) == false ) {
		errData.m_errMsg = Message::msg080(DefineRuleElementsNumber);
		errData.errorTrace(__FILE__,__LINE__-1);
		return false;
	}

	std::string sRule = getRuleFromLine(sDefine);
	if( sRule.empty() ) {
		errData.m_errMsg = Message::msg081();
		errData.errorTrace(__FILE__,__LINE__-1);
		return false;
	}

	std::string val;
	if( divideKeyAndValue(sRule,sName,val,errData) ==false ) {
		errData.errorTrace(__FILE__,__LINE__-1);
		return false;
	}

	DefineRule rule;

	if(rule.preParse(val,vRelatedDefine,errData) == false) {
		errData.errorTrace(__FILE__,__LINE__-1);
		return false;
	}

	return true;
}

bool DefineManager::parse(const LINE& line, const std::string& sType, ErrorData& errData)
{
    if( CommonParser::checkElementsNumber(line.m_line,DefineRuleElementsNumber) == false ) {
        errData.m_errMsg = Message::msg080(DefineRuleElementsNumber);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    std::string sRule = getRuleFromLine(line.m_line);
    if( sRule.empty() ) {
        errData.m_errMsg = Message::msg081();
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    std::string key,val;
    if( divideKeyAndValue(sRule,key,val,errData) ==false ) {
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    DefineRule rule;
    if(sType == "#DEFINE") {
        if(rule.parse(val,(*this),errData) == false) {
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
    } else if(sType == "#COND_DEFINE") {
        if(rule.parseConditionalDefine(val,(*this),errData) == false) {
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
    } else {
        if(rule.parseKeyDefine(val,(*this),errData) == false) {
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
    }
    rule.setLineNum(line.m_lineNum);
    m_synonymMap.insert(std::make_pair(key,rule));

    return true;
}

void DefineManager::dump(std::ostream& ofs) const
{
    ofs<<"=== DEFINE RULE: ==========================="<<std::endl;
    std::map<std::string,DefineRule>::const_iterator ite;
    for(ite=m_synonymMap.begin(); ite!=m_synonymMap.end(); ++ite) {
        ofs<<"line "<<(ite->second).getLineNum()<<":(";
        if((ite->second).getType() & BASIC_DEFINITION) {
            ofs<<"#DEFINE";
        } else if((ite->second).getType() & CONDITIONAL_DEFINITION) {
            ofs<<"#COND_DEFINE";
        } else {
            ofs<<"#DEFINE";
        }
        ofs<<"\t"<<ite->first;
        const std::vector<std::string>& v = ite->second.getVal();
        std::string val;
        val = v[0];
        for(size_t i=1; i<v.size(); i++) {
            val += "," + v[i];
        }
        ofs<<"="<<val<<")"<<std::endl;
    }
}

}//namespace sae
