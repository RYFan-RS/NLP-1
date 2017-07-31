#ifndef __ACL__STRING_UTILITY_H__
#define __ACL__STRING_UTILITY_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

namespace acl
{

namespace StringUtility{

static std::string getFileNameFromPath(const std::string& sPath)
{
    std::string sName;
    int i,tail = sPath.length()-1;

    for(i=tail; i>-1; i--) {
        if(sPath[i] == '\\' || sPath[i] == '/') {
            sName = sPath.substr(i+1);
            break;
        }
    }
    if(sName.empty()) {
        sName = sPath;
    }

    return sName;
}

static std::string getPathFromFilePath(const std::string& sFilePath)
{
    std::string sPath;
    int i,tail = sFilePath.length()-1;

    for(i=tail; i>-1; i--) {
        if(sFilePath[i] == '/' || sFilePath[i] == '\\') {
            return sFilePath.substr(0,i);
        }
    }

    return sPath;
}

static std::string generatePathWithRelativePath(const std::string& basePath, const std::string& relativePath)
{
    std::string newBasePath = getPathFromFilePath(basePath);
    if(!newBasePath.empty()) {
        return newBasePath+ '/' + relativePath;
    } else {
        return relativePath;
    }
}

static std::string getStringBetween(const std::string& s, const std::string& head, const std::string& tail)
{
    size_t hPos = s.find(head);
    if(hPos == std::string::npos) return "";
    size_t tPos = s.find(tail,hPos+head.length());
    if(tPos == std::string::npos) return "";
    return s.substr(hPos+head.length(),tPos-hPos-head.length());
}

static void switchBoolTag(bool& tag)
{
    if(tag == true) {
        tag = false;
    } else {
        tag = true;
    }
}

static bool isTranslatedChar(const std::string& s, const int& pos)
{
    bool ret = false;
    for(int i=pos-1; i>-1; i--) {
        if(s[i] == '\\') {
            switchBoolTag(ret);
        } else {
            break;
        }
    }
    return ret;
}

static std::string convertTranslateChar(const std::string& s)
{
    std::string result;

    for(int i=s.length()-1; i>-1; i--) {
        result += s[i];
        if( isTranslatedChar(s,i) ) {
            i--;
        }
    }

    std::reverse(result.begin(),result.end());

    return result;
}

static std::vector<std::string> divide(std::string input, const std::string &sep)
{
    std::vector<std::string> v;
    int pos;

    if(sep.length() == 1) { //use escape sequence
        int start = 0;
        while(1) {
            pos = input.find(sep,start);
            if(pos != (int)std::string::npos) {
                if(pos != 0) {
                    if(isTranslatedChar(input,pos) == true) {
                        start = pos + 1;
                    } else {
                        v.push_back(input.substr(0, pos));
                        input = input.substr(pos + sep.length());
                        start = 0;
                    }
                } else if(pos == (int)input.length()-1) {
                    if(isTranslatedChar(input,pos) == true) {
                        v.push_back(input);
                        break;
                    } else {
                        v.push_back(input.substr(0, pos));
                        input = input.substr(pos + sep.length());
                        start = 0;
                    }
                } else {
                    v.push_back(input.substr(0, pos));
                    input = input.substr(pos + sep.length());
                    start = 0;
                }
            } else {
                v.push_back(input);
                break;
            }
        }
    } else { //do not use escape sequence
        while(1) {
            pos = input.find(sep);
            if(pos != (int)std::string::npos) {
                v.push_back(input.substr(0, pos));
                input = input.substr(pos + sep.length());
            } else {
                v.push_back(input);
                break;
            }
        }
    }

    return v;
}

static std::string int2Str(const int& num)
{
    char cache[32];
#ifdef WIN32
    sprintf_s(cache,"%d",num);
#else
	sprintf(cache,"%d",num);
#endif
    return cache;
}

static bool isNumber(const std::string& s)
{
    for(size_t i=0; i<s.length(); i++) {
        if(s[i] == '-' || s[i] == '+') continue;
        if(s[i] < '0' || s[i] > '9') {
            return false;
        }
    }
    return true;
}

static bool isPureNumber(const std::string& s)
{
    for(size_t i=0; i<s.length(); i++) {
        if(s[i] < '0' || s[i] > '9') {
            return false;
        }
    }
    return true;
}

static void removeSpace(std::string& s)
{
    std::string str;
    for(size_t i=0; i<s.length(); i++) {
        if(s[i] != ' ') {
            str += s[i];
        }
    }
    s.swap(str);
}

static void removeSpaceNear(std::string& s, const std::string& tar)
{
    int pos = s.find(tar);
    if(pos == (int)std::string::npos) return;
    int p=-1;
    std::string str;
    size_t i;
    for(i=pos-1; i>=0; i--) {
        if(s[i] != ' ') {
            p = i;
            break;
        }
    }
    str = s.substr(0,p+1);
    str += tar;
    p=0;
    for(i=pos+tar.length(); i<s.length(); i++) {
        if(s[i] != ' ') {
            p = i;
            break;
        }
    }
    str += s.substr(p);
    s.swap(str);
}

static int hasEmptyCell(const std::vector<std::string>& v)
{
    for(size_t i=0; i<v.size(); i++) {
        if(v[i].empty()) {
            return i;
        }
    }
    return -1;
}

static std::string trimLeft(std::string& s, const std::string& sep)
{
    std::string sLeft;
    int pos = s.find(sep);
    if(pos != (int)std::string::npos) {
        sLeft = s.substr(0,pos);
        s = s.substr(pos+sep.length());
    } else {
        sLeft = s;
    }
    return sLeft;
}

static std::string toUpper(const std::string& word)
{
    std::string result;

    for(size_t i = 0; i < word.length(); i++) {
        if(word[i] > 0x40 && word[i] < 0x5b) {
            if(result.empty()) {
                result = word;
            }
            result[i] = word[i] | 0x20;
            continue;
        } else if(word[i] > 0x60 && word[i] < 0x7b) {
            continue;
        } else if(word[i] == 0x20) {
            continue;
        } else {
            return "";
        }
    }
    return result;
}

static bool isHexadecimalNumber(const std::string& s)
{
    size_t length = s.length();

    if(length < 3 || length > 10) return false;

    for(size_t i=0; i<length; i++) {
        if((s[i]<'0' || s[i]>'9') && s[i]!='x' && s[i]!='X' && (s[i]<'a' || s[i]>'f')) {
            return false;
        }
    }
    return true;
}

static void toUpper(std::string& s)
{
    for(size_t i=0; i<s.length(); i++) {
        s[i] = toupper(s[i]);
    }
}

static std::string removeBackTabsAndSpaces(const std::string& s)
{
    std::string sResult;

    for(int i=(int)s.length()-1; i>-1; i--) {
        if(s[i] != '\t' && s[i] != ' ') {
            sResult = s.substr(0,i+1);
            break;
        }
    }

    return sResult;
}

static std::string divideLineTypeComment(const std::string& s)
{
    std::string sResult;

    int pos = s.find("//");
    sResult = s.substr(0,pos);
    sResult = removeBackTabsAndSpaces(sResult);

    return sResult;
}

static int scaleNumberToLegalNumber(const int& num, const int& maxNum)
{
    if(num < 0) {
        return 0;
    } else if(num > maxNum) {
        return maxNum;
    } else {
        return num;
    }
}

static std::string getIndexCharInString(const std::string& str, const int& index)
{
    std::string word;
    int length = str.length();
    int i=0,count=0,lastPos=0;

    while(1) {
        if(i >= length)
            break;
        lastPos = i;
        if((str[i] & 0x80) == 0x00) {
            i++;
        } else if((str[i] & 0xE0) == 0xC0) {
            i+=2;
        } else if((str[i] & 0xF0) == 0xE0) {
            i+=3;
        }
        if(count == index) {
            word = str.substr(lastPos,i-lastPos);
            break;
        }
        count++;
    }

    return word;
}

static bool isStringHasLowerCaseAphabet(const std::string& str)
{
    int length = str.length();
    int i=0,count=0,lastPos=0;

    while(1) {
        if(i >= length)
            break;
        lastPos = i;
        if((str[i] & 0x80) == 0x00) {
            if('a'<= str[i] && str[i]<='z') {
                return true;
            }
            i++;
        } else if((str[i] & 0xE0) == 0xC0) {
            i+=2;
        } else if((str[i] & 0xF0) == 0xE0) {
            i+=3;
        }
        count++;
    }
    return false;
}

static int isVectorHasLowerCaseAphabet(const std::vector<std::string>& v)
{
    for(size_t i=0; i<v.size(); i++) {
        if(isStringHasLowerCaseAphabet(v[i])) {
            return i;
        }
    }
    return -1;
}

}//namespace StringUtility
}//namespace acl
#endif
