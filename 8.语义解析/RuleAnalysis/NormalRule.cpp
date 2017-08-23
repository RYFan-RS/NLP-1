#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "NormalRule.h"

namespace sae
{

void NormalRuleManager::setDataManagerPointer(DataManager* pData)
{
    for(size_t i=0; i<m_rules.size(); i++) {
        m_rules[i].setDataManagerPointer(pData);
    }
}

std::vector<std::string> NormalRuleManager::divideRuleLine(const std::string& line)
{
    std::vector<std::string> vRules;
    std::string str;

    int level=0;
    int pos;
    for(size_t i=0; i<line.length(); i++) {
        if(line[i] == '(') {
            if( Utility::isTranslatedChar(line,i) ) continue;
            if(level == 0) pos = i;
            level++;
        } else if(line[i] == ')') {
            if( Utility::isTranslatedChar(line,i) ) continue;
            level--;
            if(level == 0) {
                str = line.substr(pos,i-pos+1);
                vRules.push_back(str);
            }
        }
    }
    return vRules;
}

bool NormalRuleManager::parseTemplate(const LINE& line, DataManager* pDataManager, ErrorData& errData)
{
	const Template& tpl = pDataManager->getConstTemplate();
	
    std::vector<std::string> vRuleLine;
	ERROR_TRACE( tpl.produce(line.m_line, vRuleLine, errData) )
	
	for(size_t i=0; i<vRuleLine.size(); i++){
		std::vector<std::string> vRuleString = divideRuleLine(vRuleLine[i]);
	
		if(vRuleString.empty()) {
			ERROR_MESSAGE( Message::msg015() )
		}
		if(vRuleString.size() > 4) {
			ERROR_MESSAGE( Message::msg016() )
		}

		NormalRule cell(pDataManager);

		ERROR_TRACE( cell.parse(vRuleString,errData) )
		else {
			cell.setLineIndexID(line.m_lineNum);
			m_rules.push_back(cell);
		}
	}
	
    return true;
}

bool NormalRuleManager::parse(const LINE& line, DataManager* pDataManager, ErrorData& errData)
{
    std::vector<std::string> vRules = divideRuleLine(line.m_line);

    if(vRules.empty()) {
        ERROR_MESSAGE( Message::msg015() )
    }
    if(vRules.size() > 4) {
        ERROR_MESSAGE( Message::msg016() )
    }

    NormalRule cell(pDataManager);

    ERROR_TRACE( cell.parse(vRules,errData) )
    else {
        cell.setLineIndexID(line.m_lineNum);
        m_rules.push_back(cell);
    }

    return true;
}

bool NormalRule::getNameFromString(std::string& rule, std::string& name, ErrorData& errData)
{
    Utility::trimLeft(rule,"\t");
    name = Utility::trimLeft(rule,"\t");

	if(rule[0] != '('){
		m_idName = Utility::trimLeft(rule,"\t");
	}
    if(name.empty()) {
        ERROR_MESSAGE( Message::msg017() )
    } else return true;
}

bool NormalRule::getKeyFromString(std::string& rule, std::string& sKey, const int& keyIndex, ErrorData& errData)
{
    sKey = "";

    //$(A)  <- at least, length = 4
    if(rule.length() < 4) return true;
    if(rule[0] == '(') { //parse normal type
        int count = 1;
        for(size_t i=1; i<rule.length(); i++) {
            if(rule[i] == '(') {
                if( Utility::isTranslatedChar(rule,i) ) continue;
                count++;
            } else if(rule[i] == ')') {
                if( Utility::isTranslatedChar(rule,i) ) continue;
                count--;
                if(count == 0) {
                    sKey = rule.substr(0,i+1);
                    rule = rule.substr(i+1);
                    return true;
                }
            }
        }
    } else if(rule[0] == '$' && rule[1] == '(') { //parse key definition
        for(size_t i=2; i<rule.length(); i++) {
            if(rule[i] == ')') {
                if( Utility::isTranslatedChar(rule,i) ) continue;
                else {
                    sKey = rule.substr(2,i-2);
                    rule = rule.substr(i+1);
                    break;
                }
            }
        }

        const DefineRule& dRule = mp_DataManager->getDefineManager().find(sKey);
        const std::vector<std::string>& v = dRule.getVal();
        if(v.empty()) {
            ERROR_MESSAGE( Message::msg018(sKey) )
        }
        sKey = v[0];
        return true;
    }
    ERROR_MESSAGE( Message::msg018(sKey) )
}

const std::string NormalRule::getRuleType(const std::string& rule) const
{
    std::string sRule = rule.substr(1,rule.length()-2);
    sRule = Utility::trimLeft(sRule,"\t");
    return sRule;
}

bool NormalRule::checkWholePartElementsNumber(const std::string& rule, ErrorData& errData)
{
    size_t pos = rule.find("(",1);
    int count = 0;
    for(size_t i=0; i<pos; i++) {
        if(rule[i] == '\t') {
            count++;
        }
    }
    if((count != WholePartElementsNumber)&& (count != WholePartElementsNumber+1)) {
        ERROR_MESSAGE( Message::msg020(WholePartElementsNumber) )
    } else {
        return true;
    }
}

bool NormalRule::parse(const std::vector<std::string>& vRules, ErrorData& errData)
{
    std::string sRule = vRules[0];

    ERROR_TRACE( checkWholePartElementsNumber(sRule,errData) )

    //rule name
    std::string sName;
    ERROR_TRACE( getNameFromString(sRule,sName,errData) )
    else {
        int idx = mp_DataManager->getIndexManager().find(sName);
        if(  idx < 0 ) {
            ERROR_MESSAGE( Message::msg021(sName) )
        } else {
            m_idx = idx;
        }
    }

    //keys
    int keyIndex = 0;
    while(1) {
        std::string sKey;
        ERROR_TRACE( getKeyFromString(sRule,sKey,keyIndex,errData) )
        if( sKey.empty() ) break;

        KeyCell cell;
        ERROR_TRACE( cell.readKeyString(sKey,keyIndex,mp_DataManager,errData) )
        m_keys.addKey(cell);
        keyIndex++;
    }
	//analysis rule 0, the above code
	//-------------------------------------------------------------------------
	//analysis tag in the rule in the mark, the following code. 

    ERROR_TRACE( m_keys.checkReferOfOmissibleKeyInKeys(errData) )

    ERROR_TRACE( m_keys.checkPositionsOutsideBounds(errData) )

    size_t i;
    for(i=1; i<vRules.size(); i++) {
        std::string ruleType = getRuleType(vRules[i]);
        if(ruleType == "@TAG") { //tag
            ERROR_TRACE( m_tagRule.parse(vRules[i],mp_DataManager,errData) )
        } else {
            ERROR_MESSAGE( Message::msg022(ruleType) )
        }
    }

    //check if range limitation is conflicted with omissible keys
    if( checkKeyRangesInExecuteRulesConflictWithOmissibleKeys(m_keys.getOmissibleKeySet(),errData) == false ) {
        ERROR_MESSAGE_TRACE( Message::msg023(), ADD_TO_BACK )
    }

    std::vector<KeyCell>& keys = m_keys.getKeys();
    std::vector<int> vKeyLength(keys.size());

    //check if the keys in tag rule is out of the range of key part
    ERROR_TRACE( m_tagRule.isKeyOutOfRange(vKeyLength,errData) )

    return true;
}
bool NormalRule::preProcess(const std::vector<std::string>& vRules, ErrorData& errData,std::set<std::string>& vTags, std::set<std::string>& vIndex) 
{
    std::string sRule = vRules[0];

    ERROR_TRACE( checkWholePartElementsNumber(sRule,errData) )

    //rule name
    std::string sName;
    ERROR_TRACE( getNameFromString(sRule,sName,errData) )
    else {
		vIndex.insert(sName);
    }

    while(1) {
        std::string sKey;

	    if(sRule.length() < 4)  break;
	    if(sRule[0] == '(') { 
	        int count = 1;
	        for(size_t i=1; i<sRule.length(); i++) {
	            if(sRule[i] == '(') {
	                if( Utility::isTranslatedChar(sRule,i) ) continue;
	                count++;
	            } else if(sRule[i] == ')') {
	                if( Utility::isTranslatedChar(sRule,i) ) continue;
	                count--;
	                if(count == 0) {
	                    sKey = sRule.substr(0,i+1);
	                    sRule = sRule.substr(i+1);
	                    break;
	                }
	            }
	        }
	    } 		
		else
		{
			break;
		}
		
        if( sKey.empty() ) break;

		sKey = sKey.substr(1,sKey.length()-2);;
		std::string searchMode = Utility::trimLeft(sKey,"\t");
		if(searchMode.empty()) {
			return false;
		} 

		if(searchMode.find("T") != std::string::npos)
		{
			Utility::trimLeft(sKey,"\t");
			Utility::trimLeft(sKey,"\t");
			Utility::trimLeft(sKey,"\t");
			std::string sTag = Utility::trimLeft(sKey,"\t");
			if(sKey.empty()) return false;
			if(sKey.at(0) != '*')
			{
				vTags.insert(sKey);
			}
		}
    }

	//Tag name
    std::string ruleType = getRuleType(vRules[1]);
    if(ruleType == "@TAG") { //tag
        ERROR_TRACE( m_tagRule.preProcess(vRules[1],vTags,errData) )
    } else {
        ERROR_MESSAGE( Message::msg022(ruleType) )
    }

    return true;
}

std::string NormalRule::getName() const
{
    return mp_DataManager->getConstIndexManager().find(m_idx);
}

void NormalRule::dump(std::ostream& ofs) const
{
    ofs<<getLineIndexInfo()<<":(#NORMAL";
    ofs<<DUMP_SEPARATOR;
    ofs<<getName()<<DUMP_SEPARATOR;
    m_keys.dump(*mp_DataManager,ofs);
    ofs<<")";
    if(hasTagRule()) {
        m_tagRule.dump(*mp_DataManager,ofs);
    }
}

void NormalRuleManager::dump(std::ostream& ofs) const
{
    for(size_t i=0; i<m_rules.size(); i++) {
        m_rules[i].dump(ofs);
        ofs<<std::endl;
    }
}

std::vector<NormalRule>& NormalRuleManager::getRules()
{
    return m_rules;
}

NormalRule& NormalRuleManager::getRule(const int& index)
{
    return m_rules[index];
}

const NormalRule& NormalRuleManager::getConstRule(const int& index) const
{
    return m_rules[index];
}

bool NormalRule::checkKeyRangesInExecuteRulesConflictWithOmissibleKeys(const std::set<int>& setOmissibleKey, ErrorData& errData)
{
    if( hasTagRule() && !m_tagRule.checkKeyRangesConflictWithOmissilbeKeys(setOmissibleKey) ) {
        ERROR_MESSAGE( Message::msg024() )
    }

    return true;
}

}//namespace sae
