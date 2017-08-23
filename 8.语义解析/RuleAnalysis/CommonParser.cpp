#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "CommonParser.h"
#include <stdlib.h>

namespace sae
{

std::vector<std::string> CommonParser::divideAndCheckElements(const std::string& line, const int& stdAmount)
{
    std::vector<std::string> emptyVec;

    std::vector<std::string> vElements = Utility::divide(line,"\t");
    if((int)vElements.size() != stdAmount) {
        return emptyVec;
    }

    if( Utility::hasEmptyCell(vElements) > 0 ) {
        return emptyVec;
    }

    return vElements;
}

KeyNum CommonParser::getKeyNumFromStr(const std::string& sKey)
{
    KeyNum keyNum;
    if(sKey.length() < 3) {
        keyNum.m_keyNum = -1;
        return keyNum;
    }
    if(sKey[0] != '[' && sKey[sKey.length()-1] != ']') {
        keyNum.m_keyNum = -2;
        return keyNum;
    }
    std::string sNum = sKey.substr(1,sKey.length()-2);
    std::vector<std::string> v = Utility::divide(sNum,":");
    if( v.size() > 2 ) {
        keyNum.m_keyNum = -3;
        return keyNum;
    }
    if( Utility::hasEmptyCell(v) > 0 ) {
        keyNum.m_keyNum = -4;
        return keyNum;
    }
    if( Utility::isNumber(v[0]) == false ) {
        keyNum.m_keyNum = -5;
        return keyNum;
    } else {
        keyNum.m_keyNum = atoi(v[0].c_str());
    }
    if(v.size() == 2) {
        if( Utility::isNumber(v[1]) == false ) {
            keyNum.m_keyNum = -6;
            return keyNum;
        } else {
            keyNum.m_subKeyNum = atoi(v[1].c_str());
        }
    }
    return keyNum;
}

bool CommonParser::findAndChangeKeyNumToNum(std::string& s)
{
    std::string cache;
    std::string newStr;
    bool isKeyType = false;

    for(size_t i=0; i<s.length(); i++) {
        if(s[i] == '[') {
            if(i>0) {
                if(s[i-1]=='$' || s[i-1]=='@' || s[i-1]=='\\') {
                    newStr+=s[i];
                    continue;
                }
            }
            cache += s[i];
        } else if(s[i] == ']') {
            if(!cache.empty()) {
                newStr += cache.substr(1);
                isKeyType = true;
                cache = "";
            } else {
                newStr+=s[i];
            }
        } else {
            if(!cache.empty()) {
                cache += s[i];
            } else {
                newStr += s[i];
            }
        }
    }
    s.swap(newStr);

    return isKeyType;
}

int CommonParser::getNextOperatorPosition(const std::string& sFormula,const int& iPos)
{
    int posIntersection,posUnion;

    posIntersection = sFormula.find("&",iPos);
    posUnion = sFormula.find("/",iPos);
    if(posIntersection == (int)std::string::npos && posUnion != (int)std::string::npos) {
        return posUnion;
    } else if(posIntersection != (int)std::string::npos && posUnion == (int)std::string::npos) {
        return posIntersection;
    } else if(posIntersection != (int)std::string::npos && posUnion != (int)std::string::npos) {
        return TheMinOf(posIntersection,posUnion);
    } else {
        return -1;
    }
}

bool CommonParser::divideFormula(const std::string& sFormula,std::vector<FormulaString>& vFormulaString,const bool& needReplace)
{
    std::vector<std::string> vFormula,vOperator;
    std::set<std::string> setIntersection,setUnion;
    std::set<std::string>::iterator ite;
    int pos = -1,lastPos = 0;

    //divide by operator
    while(1) {
        pos = getNextOperatorPosition(sFormula,lastPos);
        if(pos == -1) {
            vFormula.push_back(sFormula.substr(lastPos));
            break;
        } else {
            vFormula.push_back(sFormula.substr(lastPos,pos-lastPos));
            vOperator.push_back(sFormula.substr(pos,1));
            lastPos = pos+1;
        }
    }

    //sort the formulas by type
    if(vOperator.empty()) {
        setIntersection.insert(vFormula[0]);
    } else {
        for(size_t i=0; i<vOperator.size(); i++) {
            if(vOperator[i] == "&") {
                setIntersection.insert(vFormula[i]);
                setIntersection.insert(vFormula[i+1]);
            } else {
                setUnion.insert(vFormula[i]);
                setUnion.insert(vFormula[i+1]);
            }
        }
    }

    //generate vector FormulaString
    FormulaString formulaString;
    std::string formula;
    for(ite=setIntersection.begin(); ite!=setIntersection.end(); ++ite) {
        formula = *ite;
        formulaString.m_isIntersection = true;
        formulaString.m_isKeyType = false;
        if(needReplace == true) {
            if( findAndChangeKeyNumToNum(formula) == true ) {
                formulaString.m_isKeyType = true;
            }
        }
        formulaString.m_sFormula = formula;
        vFormulaString.push_back(formulaString);
    }
    for(ite=setUnion.begin(); ite!=setUnion.end(); ++ite) {
        formula = *ite;
        formulaString.m_isIntersection = false;
        formulaString.m_isKeyType = false;
        if(needReplace == true) {
            if( findAndChangeKeyNumToNum(formula) == true ) {
                formulaString.m_isKeyType = true;
            }
        }
        formulaString.m_sFormula = formula;
        vFormulaString.push_back(formulaString);
    }

    //the first formulaString should always be intersection type
    vFormulaString[0].m_isIntersection = true;

    return true;
}

bool CommonParser::checkType(const std::string& s, char& type, std::vector<std::string>& vCell, ErrorData& errData)
{
    int selectType = 0;
    int andType = 0;
    int shuffleType = 0;
    std::string cache;
    int i,length,tail;
    int inBracket0 = 0;

    length = s.length();
    tail = length-1;
    for(i=0; i<length; i++) {
        if(s[i] == '/' && inBracket0 == 0) {
            if(Utility::isTranslatedChar(s,i) == false) {
                selectType = 1;
                vCell.push_back(cache);
                cache = "";
            } else {
                cache += s[i];
            }
        } else if(s[i] == '|' && inBracket0 == 0) {
            if(Utility::isTranslatedChar(s,i) == false) {
                shuffleType = 1;
                vCell.push_back(cache);
                cache = "";
            } else {
                cache += s[i];
            }
        } else if(s[i] == '(') {
            if(Utility::isTranslatedChar(s,i) == false) {
                inBracket0++;
            }
            cache += s[i];
        } else if(s[i] == ')') {
            if(Utility::isTranslatedChar(s,i) == false) {
                inBracket0--;
            }
            cache += s[i];
        } else {
            cache += s[i];
        }

    }
    vCell.push_back(cache);
    cache = "";

    int totalType = selectType + andType + shuffleType;
    if(totalType == 1) {
        if(selectType == 1) {
            type |= OR_RELATION;
        } else if(andType == 1) {
            type |= AND_RELATION;
        } else if(shuffleType == 1) {
            type |= SHUFFLE_RELATION;
        }
    } else if(totalType > 1) {
        errData.m_errMsg = Message::msg067();
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    return true;
}

std::vector<std::string> CommonParser::divideKeyAndContent(const std::string& sTag)
{
    std::vector<std::string> v;
    int inBracket = 0;
    int lastPos = 0;

    for(size_t i=0; i<sTag.length(); i++) {
        if(sTag[i] == '[') {
            inBracket++;
        } else if(sTag[i] == ']') {
            inBracket--;
        } else if(sTag[i] == ':' && inBracket < 1) {
            v.push_back(sTag.substr(0,i));
            lastPos = i;
        }
    }
    v.push_back(sTag.substr(lastPos+1));

    return v;
}

bool CommonParser::isOutOfRange(const KeyNum& keyNum, const int& maxKeyNum)
{
    return keyNum.m_keyNum != WHOLE_SENTENCE && (keyNum.m_keyNum<0 || keyNum.m_keyNum > maxKeyNum) ? true : false;
}

//$(S)
bool CommonParser::isSynonymExtend(const std::string& word)
{
    if(word.length() < 4) return false;

    if(word[0] == '$' && word[1] == '(' && word[word.length()-1] == ')') {
        return true;
    }
    return false;
}

bool CommonParser::getSynonymExtendKeyword(const std::string& word, std::string& functionMark, std::string& keyword, std::string& basicWordMark)
{
    size_t pos = word.find("@");
    if( pos != std::string::npos ) {
        keyword = word.substr(2,pos-2);
        functionMark = word.substr(pos+1, word.length()-pos-2);
        if(functionMark.size() == 4) {
            basicWordMark = functionMark.substr(0,3);
            functionMark = functionMark[3];
        } else if(functionMark.size() == 3) {
            basicWordMark = functionMark;
            functionMark = "";
        }
    } else {
        keyword = word.substr(2, word.length()-3);
        functionMark = "";
    }

    return true;
}

bool CommonParser::checkElementsNumber(const std::string& line, const int& number)
{
    int count = 1;
    int inBracket0 = 0;
    const size_t length = line.length();

    for(size_t i=0; i<length; i++) {
        if(line[i] == '\t' && inBracket0 == 0) {
            count++;
        } else if(i > 0 && line[i] == '(' && !Utility::isTranslatedChar(line,i) ) {
            inBracket0++;
        } else if(i < length - 1 && line[i] == ')' && !Utility::isTranslatedChar(line,i)) {
            inBracket0--;
        }
    }
    if(count == number) {
        return true;
    } else {
        return false;
    }
}

std::vector<std::string> CommonParser::divideElements(const std::string& line, const int& number,ErrorData& errData)
{
    std::vector<std::string> vElement = Utility::divide(line,"\t");

    if((int)vElement.size() != number) {
        errData.m_errMsg = Message::msg070(number);
        errData.errorTrace(__FILE__,__LINE__-1);
        vElement.clear();
        return vElement;
    }

    int ret = Utility::hasEmptyCell(vElement);
    if(ret != -1) {
        errData.m_errMsg = Message::msg071(ret);
        errData.errorTrace(__FILE__,__LINE__-1);
        vElement.clear();
        return vElement;
    }

    return vElement;
}

bool CommonParser::isSelectTypeString(const std::string& s, std::vector<std::string>& v, ErrorData& errData)
{
    v = Utility::divide(s,"/");

    int ret = Utility::hasEmptyCell(v);
    if(ret > -1) {
        errData.m_errMsg = Message::msg072(ret,s);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    return true;
}

std::string CommonParser::normalizeTailOfTheLine(const std::string& s)
{
    int i,length;
    length = s.length();
    size_t pos = std::string::npos;
    for(i=--length; i>-1; i--) {
        if(s[i] == '\0' || s[i] == '\r' || s[i] == '\n' || s[i] == '\t') {
            pos = i;
        } else {
            break;
        }
    }

    return s.substr(0,pos);
}

std::string CommonParser::removeCommentAtTail(const std::string& s)
{
    size_t pos = s.find("//");
    return pos == std::string::npos ? s : normalizeTailOfTheLine(s.substr(0,pos));
}

bool CommonParser::isReferKey(const std::string& s)
{
    if(s.length() < 3) return false;
    return s[0]=='[' && s[s.length()-1]==']' ? true : false;
}

}//namespace sae

