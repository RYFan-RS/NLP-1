#ifndef __SAE_UTILITY_H__
#define __SAE_UTILITY_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace sae
{

class Utility
{
public:
    static std::string getFileNameFromPath(const std::string& sPath);
    static std::string getPathFromFilePath(const std::string& sFilePath);
    static std::string generatePathWithRelativePath(const std::string& basePath, const std::string& relativePath);
    static std::string getStringBetween(const std::string& s, const std::string& head, const std::string& tail);
    static void switchBoolTag(bool& tag);
    static bool isTranslatedChar(const std::string& s, const int& pos);
    static std::string convertTranslateChar(const std::string& s);
    static std::vector<std::string> divide(std::string input, const std::string &sep);
    static std::string int2Str(const int& num);
    static bool isNumber(const std::string& s);
    static bool isPureNumber(const std::string& s);
    static void removeSpaceNear(std::string& s, const std::string& tar);
    static int hasEmptyCell(const std::vector<std::string>& v);
    static std::string trimLeft(std::string& s, const std::string& sep);
    static std::string toUpper(const std::string& word);
    static bool isHexadecimalNumber(const std::string& s);
    static void toUpper(std::string& s);
    static std::string divideLineTypeComment(const std::string& s);
    static int scaleNumberToLegalNumber(const int& num, const int& maxNum);
    static std::string getIndexCharInString(const std::string& str, const int& index);
    static bool isStringHasLowerCaseAphabet(const std::string& str);
    static int isVectorHasLowerCaseAphabet(const std::vector<std::string>& v);
//unused:
    static void removeSpace(std::string& s);
private:
    static std::string removeBackTabsAndSpaces(const std::string& s);

};

}//namespace sae

#endif//__MEAN_UTILITY_H__
