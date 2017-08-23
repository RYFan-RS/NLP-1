#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "CommonParser.h"
#include "Utility.h"
#include "Position.h"
#include <stdlib.h>

namespace sae
{

PosCell::PosCell()
{
    m_isIntersection = true;
    m_isKeyType = false;
    m_hDirection = NO_DIRECTION;
    m_num = -1;
    m_whichPosOfKey = -1;
    m_tDirection = NO_DIRECTION;
    m_offset = 0;
}

bool Position::isLegalFormula(const FormulaString& formulaString) const
{
    std::string legalChars = "+-0123456789HTP[]:";
    for(size_t i=0; i<formulaString.m_sFormula.length(); i++) {
        if( legalChars.find(formulaString.m_sFormula[i]) == std::string::npos ) {
            return false;
        }
    }

    return true;
}

bool Position::checkImpossibleCombination(const std::string& s, ErrorData& errData) const
{
    if(s.find("+T")!=std::string::npos || s.find("-H")!=std::string::npos) {
        errData.m_errMsg += Message::msg081(s);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }
    if(s.find("+P")!=std::string::npos || s.find("-P")!=std::string::npos) {
        errData.m_errMsg += Message::msg082(s);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }
    if(s=="H-" || s=="T+") {
        errData.m_errMsg += Message::msg083(s);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    return true;
}

std::vector<std::string> Position::divideFormulaElement(const std::string& s, ErrorData& errData) const
{
    std::vector<std::string> v,emptyV;
    std::string cache;
    bool lastIsMark = false;

    for(size_t i=0; i<s.length(); i++) {
        if(s[i] == '+' || s[i] == '-') {
            if(lastIsMark == true) {
                errData.m_errMsg += Message::msg084();
                errData.errorTrace(__FILE__,__LINE__-1);
                return emptyV;
            }
            if(cache.empty() && !v.empty()) {
                errData.m_errMsg += Message::msg084();
                errData.errorTrace(__FILE__,__LINE__-1);
                return emptyV;
            }
            if(!cache.empty()) {
                v.push_back(cache);
                cache = "";
            }
            cache += s[i];
            v.push_back(cache);
            cache = "";
            lastIsMark = true;
        } else {
            cache += s[i];
            lastIsMark = false;
        }
    }
    if(lastIsMark == false) {
        if(cache.empty()) {
            errData.m_errMsg += Message::msg084();
            errData.errorTrace(__FILE__,__LINE__-1);
            return emptyV;
        }
        v.push_back(cache);
        cache = "";
    }

    return v;
}

std::vector<PositionElement> Position::parseElement(std::vector<std::string>& v, const int& keyIndex, ErrorData& errData) const
{
    std::vector<PositionElement> vElements,emptyV;

    for(size_t i=0; i<v.size(); i++) {
        PositionElement e;
        if(v[i] == "+") {
            e.isMark = true;
            e.isPlusMark = true;
            e.num = -1;
        } else if(v[i] == "-") {
            e.isMark = true;
            e.isPlusMark = false;
            e.num = -1;
        } else {
            e.isMark = false;
            if(v[i][0] == '[') {
                if(v[i] == "[P]") {
                    v[i] = "[" + Utility::int2Str(keyIndex-1) + "]";
                }
                KeyNum k;
                if(k.parse(v[i],errData)==false) {
                    errData.errorTrace(__FILE__,__LINE__-1);
                    return emptyV;
                }
                if(k.getKeyNum() >= keyIndex) {
                    errData.m_errMsg = Message::msg085(keyIndex);
                    errData.errorTrace(__FILE__,__LINE__-1);
                    return emptyV;
                }
                e.isKey = true;
                e.keyNum = k;
            } else {
                e.isKey = false;
                if(v[i] == "H") {
                    e.num = 0;
                } else if(v[i] == "T") {
                    e.num = TAIL;
                } else {
                    if(Utility::isPureNumber(v[i])==false) {
                        errData.m_errMsg += Message::msg084();
                        errData.errorTrace(__FILE__,__LINE__-1);
                        return emptyV;
                    }
                    e.num = atoi(v[i].c_str());
                }
            }
        }
        vElements.push_back(e);
    }

    return vElements;
}

bool Position::parseFormulaElement(const std::string& s, std::vector<PositionElement>& vElements, const int& keyIndex, ErrorData& errData) const
{
    if( checkImpossibleCombination(s,errData) == false ) {
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    std::vector<std::string> v = divideFormulaElement(s,errData);
    if(v.empty()) {
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    vElements = parseElement(v,keyIndex,errData);
    if(vElements.empty()) {
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    return true;
}

bool Position::readFormula(const FormulaString& formulaString, PosCell& cell, const int& keyIndex, ErrorData& errData)
{
    std::string s = formulaString.m_sFormula;

    //count the numbers of type
    std::vector<PositionElement> vElements;
    if(parseFormulaElement(s,vElements,keyIndex,errData) == false) {
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    int count = vElements.size();
    if(count <0 || count > 3) {
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    //single type : +,-,12,H,T,[K:M]
    if(count == 1) {
        if(vElements[0].isMark == true) {
            if(vElements[0].isPlusMark == true) {
                cell.m_hDirection = DIRECTION_AFTER;
            } else {
                cell.m_hDirection = DIRECTION_BEFORE;
            }
        } else {
            cell.m_hDirection = NO_DIRECTION;
            if(vElements[0].isKey == true) {
                cell.m_isKeyType = true;
                cell.m_num = vElements[0].keyNum.m_keyNum;
                cell.m_whichPosOfKey = vElements[0].keyNum.m_subKeyNum;
            } else {
                cell.m_num = vElements[0].num;
            }
        }
    }
    //double types : +12,-12,+H,-T,H+,T-,[K:M]+
    else if(count == 2) {
        if(vElements[0].isMark == true) {
            cell.m_tDirection = NO_DIRECTION;
            if(vElements[0].isPlusMark == true) {
                cell.m_hDirection = DIRECTION_AFTER;
            } else {
                cell.m_hDirection = DIRECTION_BEFORE;
            }
            if(vElements[1].num == 0 || vElements[1].num == TAIL) {
                cell.m_num = 1;//+H  -T
            } else {
                cell.m_num = vElements[1].num;
            }
        } else {
            cell.m_hDirection = NO_DIRECTION;
            if(vElements[1].isPlusMark == true) {
                cell.m_tDirection = DIRECTION_AFTER;
            } else {
                cell.m_tDirection = DIRECTION_BEFORE;
            }
            if(vElements[0].isKey == true) {
                cell.m_isKeyType = true;
                cell.m_num = vElements[0].keyNum.m_keyNum;
                cell.m_whichPosOfKey = vElements[0].keyNum.m_subKeyNum;
            } else {
                cell.m_num = vElements[0].num;
            }
        }
    }
    //triple types : +12+,-12-,-T-,+H+,[1]+1
    else if(count == 3) {
        if(vElements[0].isMark == true) { //type:+12+,-12-,-T-,+H+
            if(vElements[1].isKey == true) { //type: +[K:M]+
                errData.m_errMsg += Message::msg086();
                errData.errorTrace(__FILE__,__LINE__-1);
                return false;
            }
            if(vElements[0].isPlusMark == true) {
                cell.m_hDirection = DIRECTION_AFTER;
            } else {
                cell.m_hDirection = DIRECTION_BEFORE;
            }
            if(vElements[2].isPlusMark == true) {
                cell.m_tDirection = DIRECTION_AFTER;
            } else {
                cell.m_tDirection = DIRECTION_BEFORE;
            }
            if(vElements[1].num == 0 || vElements[1].num == TAIL) {
                cell.m_num = 1;
            } else {
                cell.m_num = vElements[1].num;
            }
        } else { //type:[1]+1,[1]-1,T-1
            if(vElements[0].isKey == false && vElements[0].num != TAIL) {
                errData.m_errMsg += Message::msg087();
                errData.errorTrace(__FILE__,__LINE__-1);
                return false;
            }
            cell.m_hDirection = NO_DIRECTION;
            if(vElements[1].isPlusMark == true) {
                cell.m_tDirection = DIRECTION_AFTER;
            } else {
                cell.m_tDirection = DIRECTION_BEFORE;
            }
            if(vElements[0].num == TAIL) {
                cell.m_isKeyType = false;
                cell.m_num = TAIL;
            } else {
                cell.m_isKeyType = true;
                cell.m_num = vElements[0].keyNum.m_keyNum;
                cell.m_whichPosOfKey = vElements[0].keyNum.m_subKeyNum;
            }
            if(vElements[2].num == 0 || vElements[2].num == TAIL) {
                cell.m_offset = 1;
            } else {
                cell.m_offset = vElements[2].num;
            }
        }
    }
    cell.m_isIntersection = formulaString.m_isIntersection;

    return true;
}

int Position::readFormulas(std::vector<FormulaString>& vFormulaString, const int& keyIndex, ErrorData& errData)
{
    for(size_t i=0; i<vFormulaString.size(); i++) {
        if( vFormulaString.empty() ) {
            errData.m_errMsg = Message::msg088(keyIndex);
            errData.errorTrace(__FILE__,__LINE__-1);
            return 1;
        }
        if( isLegalFormula(vFormulaString[i]) == false ) {
            errData.m_errMsg = Message::msg089(keyIndex);
            errData.errorTrace(__FILE__,__LINE__-1);
            return 1;
        }
        //read formula
        PosCell cell;
        if( readFormula(vFormulaString[i],cell,keyIndex,errData) == false ) {
            errData.m_errMsg = Message::msg090(keyIndex) + errData.m_errMsg;
            errData.errorTrace(__FILE__,__LINE__-1);
            return 3;
        }

        m_searchPos.push_back(cell);
    }

    if(isImpossiblePosition()) {
        errData.m_errMsg = Message::msg091(keyIndex);
        errData.errorTrace(__FILE__,__LINE__-1);
        return 1;
    }

    return 0;
}

int Position::readPositionString(const std::string& sPosition, const int& keyIndex, ErrorData& errData)
{
    std::vector<FormulaString> vFormulaString;

    if(sPosition.find("+[") != std::string::npos || sPosition.find("-[") != std::string::npos) {
        errData.m_errMsg = Message::msg092(keyIndex);
        errData.errorTrace(__FILE__,__LINE__-1);
        return 2;
    }

    if( CommonParser::divideFormula(sPosition,vFormulaString) == false ) {
        errData.m_errMsg = Message::msg093(keyIndex);
        errData.errorTrace(__FILE__,__LINE__-1);
        return 1;
    }

    if( readFormulas(vFormulaString,keyIndex,errData) != 0 ) {
        errData.errorTrace(__FILE__,__LINE__-1);
        return 5;
    }

    return 0;
}

bool Position::isAllPositionInSet(std::set<int>& setKey) const
{
    KeyNum keyNum;

    for(size_t i=0; i<m_searchPos.size(); i++) {
        if(m_searchPos[i].getReferKeyNum(keyNum) == true) {
            if(setKey.find(keyNum.m_keyNum) == setKey.end()) {
                return false;
            }
        } else {
            return false;
        }
    }

    return true;
}

void Position::dump(std::ostream& ofs) const
{
    if(isSearchAnywhere() == true) {
        ofs<<"*";
    } else {
        for(size_t i=0; i<m_searchPos.size(); i++) {
            if(i != 0) {
                if(m_searchPos[i].m_isIntersection == true) {
                    ofs<<"&";
                } else {
                    ofs<<"/";
                }
            }
            if(m_searchPos[i].m_hDirection == DIRECTION_BEFORE) {
                ofs<<"-";
            } else if(m_searchPos[i].m_hDirection == DIRECTION_AFTER) {
                ofs<<"+";
            }
            if(m_searchPos[i].m_isKeyType == false) {
                if(m_searchPos[i].m_num == 0) {
                    ofs<<"H";
                } else if(m_searchPos[i].m_num == TAIL) {
                    ofs<<"T";
                } else {
                    if(m_searchPos[i].m_num > -1) {
                        ofs<<m_searchPos[i].m_num;
                    }
                }
            } else {
                ofs<<"["<<m_searchPos[i].m_num;
                if(m_searchPos[i].m_whichPosOfKey > -1) {
                    if(m_searchPos[i].m_whichPosOfKey == 0) {
                        ofs<<":H";
                    } else if(m_searchPos[i].m_whichPosOfKey == TAIL) {
                        ofs<<":T";
                    } else {
                        ofs<<":"<<m_searchPos[i].m_whichPosOfKey;
                    }
                }
                ofs<<"]";
            }
            if(m_searchPos[i].m_tDirection == DIRECTION_BEFORE) {
                ofs<<"-";
            } else if(m_searchPos[i].m_tDirection == DIRECTION_AFTER) {
                ofs<<"+";
            }
            if(m_searchPos[i].m_offset != 0) {
                ofs<<m_searchPos[i].m_offset;
            }
        }
    }
}
bool PosCell::getReferKeyNum(KeyNum& keyNum) const
{
    if(m_isKeyType == true) {
        keyNum.m_keyNum = m_num;
        keyNum.m_subKeyNum = m_whichPosOfKey;
        return true;
    } else {
        return false;
    }
}

int PosCell::instantiateTail(const int& tail) const
{
    return m_num > TAIL_RANGE ? (tail + m_num - TAIL) : m_num;
}

bool PosCell::isBackwardSearch() const
{
    return (m_tDirection == DIRECTION_BEFORE && m_offset == 0)
           || (m_hDirection == DIRECTION_BEFORE && m_num == -1) ? true : false;
}

bool Position::isImpossiblePosition() const
{
    size_t i;
    int key_types = 0;
    int nonkey_types = 0;
    for(i=0; i<m_searchPos.size(); i++) {
        if(m_searchPos[i].m_isIntersection==true && m_searchPos[i].m_isKeyType==true && m_searchPos[i].m_offset!=0) {
            key_types++;
        } else if(m_searchPos[i].m_isIntersection==true && m_searchPos[i].m_isKeyType==false && m_searchPos[i].m_num>-1 && m_searchPos[i].m_num<TAIL && m_searchPos[i].m_hDirection==NO_DIRECTION && m_searchPos[i].m_tDirection==NO_DIRECTION) {
            nonkey_types++;
        }
    }
    if(key_types>1 || nonkey_types>1) {
        return true;
    } else
        return false;
}

bool Position::isSinglePosition() const
{
    if(m_searchPos.size() == 1) {
        if(m_searchPos[0].m_isKeyType == true && m_searchPos[0].m_offset != 0)
            return true;
        if(m_searchPos[0].m_isKeyType == false && m_searchPos[0].m_num > -1)
            return true;
        if(m_searchPos[0].m_isKeyType == false && m_searchPos[0].m_num != -1 && m_searchPos[0].m_offset != 0)
            return true;
    }

    return false;
}

bool Position::allIntersection() const
{
	for(size_t i=0; i<m_searchPos.size(); i++) {
		if(m_searchPos[i].m_isIntersection == false) return false;
	}
	return true;
}

}//namespace sae
