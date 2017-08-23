#include "KeyRange.h"
#include <stdlib.h>

namespace sae
{

const KeyNum KeyNum::Error;

bool KeyNum::parse(const std::string& sKey, ErrorData& errData)
{
    if(sKey.length() < 3) {
        errData.m_errMsg = Message::msg052(sKey);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }
    if(sKey[0] != '[' && sKey[sKey.length()-1] != ']') {
        errData.m_errMsg = Message::msg052(sKey);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }
    std::string sNum = sKey.substr(1,sKey.length()-2);
    std::vector<std::string> v = Utility::divide(sNum,":");
    if( v.size() > 2 ) {
        errData.m_errMsg = Message::msg053(sKey);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }
    if( Utility::hasEmptyCell(v) > -1 ) {
        errData.m_errMsg = Message::msg054(sKey);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }
    if( Utility::isPureNumber(v[0]) == false && v[0] != "H" && v[0] != "T" && v[0] != "S") {
        errData.m_errMsg = Message::msg055(sKey);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    } else {
        if(v[0] == "H") {
            m_keyNum = 0;
        } else if(v[0] == "T") {
            m_keyNum = TAIL;
        } else if(v[0] == "S") {
            m_keyNum = WHOLE_SENTENCE;
        } else {
            m_keyNum = atoi(v[0].c_str());
        }
    }
    if(v.size() == 2) {
        if(v[1] == "H") {
            m_subKeyNum = 0;
        } else if(v[1] == "T") {
            m_subKeyNum = TAIL;
        } else {
            if( Utility::isPureNumber(v[1]) == false ) {
                errData.m_errMsg = Message::msg056(sKey);
                errData.errorTrace(__FILE__,__LINE__-1);
                return false;
            } else {
                m_subKeyNum = atoi(v[1].c_str());
            }
        }
    }
    return true;
}

bool KeyRange::parse(const std::string& sKeyRange, ErrorData& errData)
{
    std::vector<std::string> v = Utility::divide(sKeyRange,"-");
    if( v.size() > 2 ) {
        errData.m_errMsg = Message::msg053(sKeyRange);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    if( Utility::hasEmptyCell(v) > -1 ) {
        errData.m_errMsg = Message::msg057(sKeyRange);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    if(v.size() == 1) { //single KEY
        if(m_startKeyNum.parse(v[0],errData) == false) {
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
    } else { //KEY range
        if(m_startKeyNum.parse(v[0],errData) == false) {
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
        if(m_endKeyNum.parse(v[1],errData) == false) {
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
        m_isRange = true;
    }

    return true;
}

void KeyRange::dump(std::ostream& ofs) const
{
    if(m_isRange == true) {
        m_startKeyNum.dump(ofs);
        ofs<<"-";
        m_endKeyNum.dump(ofs);
    } else {
        m_startKeyNum.dump(ofs);
    }
}

void KeyNum::dump(std::ostream& ofs) const
{
    ofs<<"[";
    if(isWholeSentence()) {
        ofs<<"S";
    } else {
        ofs<<m_keyNum;
        if(m_subKeyNum == 0) {
            ofs<<":H";
        } else if(m_subKeyNum == TAIL) {
            ofs<<":T";
        } else if(m_subKeyNum > TAIL_RANGE) {
            ofs<<":T-"<<TAIL - TAIL_RANGE;
        } else if(m_subKeyNum > -1) {
            ofs<<":"<<m_subKeyNum;
        }
    }
    ofs<<"]";
}

bool KeyRange::ifIncludeOmissibleKeyIntSet(const std::set<int>& setOmissibleKey) const
{
    if(m_isRange == false) return false;
    std::set<int>::const_iterator ite;

    for(ite=setOmissibleKey.begin(); ite!=setOmissibleKey.end(); ++ite) {
        if(m_startKeyNum.m_keyNum<=(*ite) && (*ite)<=m_endKeyNum.m_keyNum) {
            return true;
        }
    }

    return false;
}

bool KeyRange::ifIncludeKeyIntSet(const std::set<int>& setKey) const
{
    if(m_isRange) {
        std::set<int>::const_iterator ite;

        for(ite=setKey.begin(); ite!=setKey.end(); ++ite) {
            if(m_startKeyNum.m_keyNum<=(*ite) && (*ite)<=m_endKeyNum.m_keyNum) {
                return true;
            }
        }
    } else {
        if(setKey.find(m_startKeyNum.m_keyNum) != setKey.end()) {
            return true;
        }
    }

    return false;
}

bool KeyNum::isKeyNumOverMaxKeyNum(const int& maxKeyNum) const
{
    return m_keyNum > maxKeyNum ? true : false;
}

bool KeyRange::isKeyNumOverMaxKeyNum(const int& maxKeyNum) const
{
    if(m_isRange == true) {
        return m_endKeyNum.isKeyNumOverMaxKeyNum(maxKeyNum);
    } else {
        return m_startKeyNum.isKeyNumOverMaxKeyNum(maxKeyNum);
    }
}

bool KeyNum::isSubKeyNumOverKeyLength(const std::vector<int>& vKeyLength) const
{
    if(m_subKeyNum >= vKeyLength[m_keyNum] && m_subKeyNum != TAIL) {
        return true;
    } else {
        return false;
    }
}

bool KeyRange::isSubKeyNumOverKeyLength(const std::vector<int>& vKeyLength) const
{
    if(m_isRange == true) {
        if(m_startKeyNum.isSubKeyNumOverKeyLength(vKeyLength) == true) {
            return true;
        }
        if(m_endKeyNum.isSubKeyNumOverKeyLength(vKeyLength) == true) {
            return true;
        }
    } else {
        return m_startKeyNum.isSubKeyNumOverKeyLength(vKeyLength);
    }

    return false;
}

bool KeyNum::isWholeSentence() const
{
    return m_keyNum == WHOLE_SENTENCE ? true : false;
}

bool KeyRange::isWholeSentence() const
{
    return m_startKeyNum.isWholeSentence();
}

}//namespace sae
