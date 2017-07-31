/*
  ATLAN C++ Library: Utility.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__UTILITY_HPP__
#define __ACL__UTILITY_HPP__
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include "String.hpp"
namespace acl{

class Utility{
public:
	static std::vector<std::string> separate(const std::string& s, const std::string& separator){
		std::vector<std::string> v;
		size_t pos,beg = 0;
		
		for(;;){
			pos = s.find(separator,beg);
			v.push_back(s.substr(beg, pos-beg));
			beg = pos + separator.length();
			if(pos == std::string::npos) break;
		}
		
		return v;
	}
	
	static std::map<std::string, std::string> separate(const std::string& s, const std::string& sp1, const std::string& sp2){
		acl::String sSrc(s);
		std::vector<acl::String> vGroups = sSrc.separate_(sp1);
		std::vector<acl::String>::const_iterator last = vGroups.end();
		std::vector<acl::String>::const_iterator itr = vGroups.begin();
		std::map<std::string, std::string> mRet;
		std::pair< std::map<std::string, std::string>::iterator,bool > insertRet;
		for(; itr != last; ++itr){
			std::vector<std::string> vItems = itr->separate(sp2);
			if (vItems.size() != 2){
				mRet.clear();
				break;
			}
			
			insertRet = mRet.insert(std::make_pair(vItems[0], vItems[1]));
			if(!insertRet.second){
				mRet.clear();
				break;
			}
		}
		
		return mRet;
	}
	
	static bool isAlphaNumber(char c){
		if('0'<=c && c<='9'){
			return true;
		}
		if('a'<=c && c<='z'){
			return true;
		}
		if('A'<=c && c<='Z'){
			return true;
		}
		return false;
	}
	
	static bool isAlphaNumber(const std::string& s){
		for(size_t i=0; i<s.length(); i++){
			if(isAlphaNumber(s[i]) == false){
				return false;
			}
		}
		
		return true;
	}
	
	static bool isNumber(const std::string& s){
		int nDotCount = 0;
		for(size_t i=0; i<s.length(); i++){
			if(s[i] >= '0' && s[i] <= '9'){
			}else if(s[i] == '.'){
				if(++nDotCount > 1) return false;
			}else{
				return false;
			}
		}
		
		return true;
	}
	
	static bool isAlphabet(const std::string& s){
		for(size_t i=0; i<s.length(); i++){
			if(s[i] >= 'a' && s[i] <= 'z'){
				continue;
			}else if(s[i] >= 'A' && s[i] <= 'Z'){
				continue;
			}else{
				return false;
			}
		}
		
		return true;
	}
	
	static std::string& stringReplace(std::string& source, const std::string& from, const std::string& to){
		size_t beg = 0, pos = std::string::npos;
		for(;;){
			pos = source.find(from,beg);
			if(pos == std::string::npos) 
				break;
			source = source.substr(0, pos) + to + source.substr(pos + from.length());
			beg = pos + to.length();
		}
		return source;
	}

    static int stringReplaceWithCount(std::string& source, const std::string& from, const std::string& to){
		size_t beg = 0, pos = std::string::npos;
        int count = 0;
		for(;;){
			pos = source.find(from,beg);
			if(pos == std::string::npos) 
				break;
			source = source.substr(0, pos) + to + source.substr(pos + from.length());
			beg = pos;
            count ++;
		}
		return count;
	}
	
	static size_t stringReplaceWithPosition(std::string& source, const std::string& from, const std::string& to){
		size_t pos = source.find(from);
		
		if(pos != std::string::npos){
			source = source.substr(0, pos) + to + source.substr(pos + from.length());
		}
		
		return pos;
	}
	
    static std::string& toUpper(std::string& source) {
        for(size_t i=0;i<source.size();i++){
			if(islower((source)[i])){
				(source)[i] = toupper((source)[i]);
			}
		}
        return source;
        }
	static std::string toUpper(const std::string& sIn) {
		std::string source(sIn);
        for(size_t i=0;i<source.size();i++){
			if(islower((source)[i])){
				(source)[i] = toupper((source)[i]);
			}
		}
        return source;
        }
		
    static std::string& trim(std::string& source){

        std::string::size_type pos = source.find_last_not_of(' ');
        if(pos != std::string::npos)
            {
            source.erase(pos + 1);
            pos = source.find_first_not_of(' ');
            if(pos != std::string::npos) 
                source.erase(0, pos);
            }
        else 
            source.erase(source.begin(), source.end());

        return source;
	}

    static bool isBeginWith(const std::string& source, const std::string& s){
		if(source.empty() || s.empty() || s.length() > source.length()) return false;
		
		for(size_t i=0; i<s.length(); i++){
			if(source[i] != s[i]) return false;
		}
		
		return true;
	}
	
	static 	std::string readNextSection(const std::string& s, const std::string& sBegMark, const std::string& sEndMark, int& nPos){
		size_t nBeg = s.find(sBegMark,nPos);
		if(nBeg == std::string::npos) return "";
		
		size_t nEnd = s.find(sEndMark,nBeg);
		if(nEnd == std::string::npos) return "";
		
		nPos = nEnd+sEndMark.length();
		std::string sSection = s.substr(nBeg,nPos-nBeg);
		
		return sSection;
	}
	
	static 	std::string readNextSection(const std::string& s, char cBegMark, char cEndMark, int& nPos){
		size_t nBeg,nEnd;
		
		nBeg = nPos;
		while(1){
			nBeg = s.find(cBegMark,nBeg);
			if(nBeg == std::string::npos) return "";
			if(!isEscapedChar(s,nBeg)){
				break;
			}else{
				nBeg++;
			}
		}
		
		nEnd = nBeg;
		while(1){
			nEnd = s.find(cEndMark,nEnd);
			if(nEnd == std::string::npos) return "";
			if(!isEscapedChar(s,nEnd)){
				break;
			}else{
				nEnd++;
			}
		}
		
		nPos = nEnd + 1;
		std::string sSection = s.substr(nBeg,nPos-nBeg);
		
		return sSection;
	}
	
	static size_t findChar(const std::string& s, char c, size_t uPos){
		while(1){
			uPos = s.find(c,uPos);
			if(uPos == std::string::npos) return std::string::npos;
			if(!isEscapedChar(s,uPos)){
				break;
			}else{
				uPos++;
			}
		}
		return uPos;
	}
	static std::string extractAlphaNumberString(std::string sInput) {
		std::string res;
		String curStr = "";
		for(size_t i=0;i<sInput.length();i++){
			bool isfind = false;
			while (i<sInput.length() && (('A' <= sInput.at(i) &&sInput.at(i) <= 'Z') || ('a' <= sInput.at(i) && sInput.at(i) <= 'z') 
				|| ('0' <= sInput.at(i) && sInput.at(i) <= '9') || sInput.at(i) == '#' || sInput.at(i) == '*')){
					isfind = true;
					curStr += sInput.at(i++);
			}
			if(isfind){
                return curStr;
			}
		}
		return curStr;
	} 	
	static bool isEscapedChar(const std::string& s, const int& pos){
		bool ret = false;
		
		for(int i=pos-1;i>-1;i--){
			if(s[i] == '\\') switchBoolTag(ret);
			else break;
		}
		
		return ret;
	}
	
	static void switchBoolTag(bool& tag){
		tag == true ? tag = false : tag = true;
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
};

}//namespace acl

#endif //__ACL__UTILITY_HPP__
