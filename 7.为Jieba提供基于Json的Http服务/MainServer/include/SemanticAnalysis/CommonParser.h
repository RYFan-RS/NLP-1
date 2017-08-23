#ifndef __SAE_COMMON_PARSER_H__
#define __SAE_COMMON_PARSER_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "ErrorData.h"
#include "TypeDefine.h"
#include "Utility.h"
#include "KeyRange.h"

namespace sae
{

class CommonParser
{
private:
    static bool findAndChangeKeyNumToNum(std::string& s);
    static int getNextOperatorPosition(const std::string& sFormula,const int& iPos);

public:
    static bool divideFormula(const std::string& sFormula,std::vector<FormulaString>& vFormulaString,const bool& needReplace = false);
    static bool checkType(const std::string& s, char& type, std::vector<std::string>& vCell, ErrorData& errData);

    static std::vector<std::string> divideKeyAndContent(const std::string& sTag);
    static bool isOutOfRange(const KeyNum& keyNum, const int& maxKeyNum);
    static bool checkElementsNumber(const std::string& line, const int& number);
    static std::vector<std::string> divideElements(const std::string& line, const int& number, ErrorData& errData);

    static bool isReferKey(const std::string& s);

    //$(S)
    static bool isSynonymExtend(const std::string& word);
    static bool getSynonymExtendKeyword(const std::string& word, std::string& functionMark, std::string& keyword, std::string& basicWordMark);

    static std::string normalizeTailOfTheLine(const std::string& s);
    static std::string removeCommentAtTail(const std::string& s);

//unused:
    static std::vector<std::string> divideAndCheckElements(const std::string& line, const int& stdAmount);
    static KeyNum getKeyNumFromStr(const std::string& sKey);
    static bool isSelectTypeString(const std::string& s, std::vector<std::string>& v, ErrorData& errData);

};

}//namespace sae

#endif//__MEAN_COMMON_PARSER_H__
