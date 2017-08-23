#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "Utility.h"
#include "CommonParser.h"
#include "ScriptLimit.h"
#include "Common/lib4/Regex.hpp"
#include "Common/lib4/Definations.hpp"

namespace sae
{

bool ScriptLimitCell::match(const MeanWords& words, const RANGE& range,const DataManager& dataManager) const
{
    if(wordRange.isRange()) {
        std::cout<<"We can't deal this type yet."<<std::endl;
    } else {
        int wordIndex,charIndex;
        std::string strReturn;

        wordIndex = range.start + Utility::scaleNumberToLegalNumber(wordRange.getStartKeyNum().getKeyNum(),range.end);
        std::string word = words[wordIndex].getStrWord(dataManager);
        if(wordRange.getStartKeyNum().getSubKeyNum() > -1) {
            charIndex = Utility::scaleNumberToLegalNumber(wordRange.getStartKeyNum().getSubKeyNum(),word.length()-1);
            word = word[charIndex];
        }
        for(size_t i=0; i<patterns.size(); i++) {
			std::vector<std::string> matchItems;
            acl::Regex regex;
			
			regex.match(patterns[i],word,&matchItems);
			if(matchItems.size() != 1 || matchItems[0].length() != word.length()){
				return false;
			}
        }
    }

    return true;
}

bool ScriptLimitCell::parse(const std::string& str, DataManager* pDataManager, ErrorData& errData)
{
    std::vector<std::string> v = CommonParser::divideKeyAndContent(str);

    if(Utility::hasEmptyCell(v) > -1 || v.size() != 2) {
        errData.m_errMsg = Message::msg097();
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    if(wordRange.parse(v[0],errData) == false) {
        errData.m_errMsg = Message::msg097() + errData.m_errMsg;
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    if(CommonParser::isSynonymExtend(v[1]) == true) {
        std::string keyWord,functionMark,basicWordMark;
        CommonParser::getSynonymExtendKeyword(v[1],functionMark,keyWord,basicWordMark);
        const DefineRule& dRule = pDataManager->getDefineManager().find(keyWord);
        if( !(dRule.getType() & BASIC_DEFINITION) ) {
            errData.m_errMsg = Message::msg098(keyWord);
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
        const std::vector<std::string>& synonymValues = dRule.getVal();
        if(synonymValues.empty()) {
            errData.m_errMsg = Message::msg061(keyWord);
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
        for(size_t i=0; i<synonymValues.size(); i++) {
            patterns.push_back(synonymValues[i]);
        }
    } else {
        patterns.push_back(v[1]);
    }

    return true;
}

void ScriptLimitCell::dump(std::ostream& ofs) const
{
    wordRange.dump(ofs);
    ofs<<":";
    for(size_t i=0; i<patterns.size(); i++) {
        if(i != 0) ofs<<"/";
        ofs<<patterns[i];
    }
}

bool ScriptLimit::match(const MeanWords& words, const DataManager& dataManager, const RANGE& range) const
{
    if(cells.empty()) return true;

    for(size_t i=0; i<cells.size(); i++) {
        if(cells[i].match(words,range,dataManager) == false) {
#ifdef MEAN_DEBUG
            ssDebug << Message::msg099();
            acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
			ssDebug.str("");
#endif //MEAN_DEBUG
            return false;
        }
    }

#ifdef MEAN_DEBUG
    ssDebug << Message::msg100();
    acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
	ssDebug.str("");
#endif //MEAN_DEBUG

    return true;
}

bool ScriptLimit::hasLimit() const
{
    return cells.empty() ? false : true;
}

bool ScriptLimit::parse(const std::string& str, DataManager* pDataMaanger, ErrorData& errData)
{
    std::vector<std::string> v = Utility::divide(str,";");

    if(Utility::hasEmptyCell(v) > -1) {
        errData.m_errMsg = Message::msg097();
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }
    for(size_t i=0; i<v.size(); i++) {
        ScriptLimitCell cell;
        if(cell.parse(v[i],pDataMaanger,errData) == false) {
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
        cells.push_back(cell);
    }

    return true;
}

void ScriptLimit::dump(std::ostream& ofs) const
{
    if(cells.empty()) {
        ofs<<"*";
    } else {
        for(size_t i=0; i<cells.size(); i++) {
            if(i != 0) ofs<<";";
            cells[i].dump(ofs);
        }
    }
}

}//namespace sae
