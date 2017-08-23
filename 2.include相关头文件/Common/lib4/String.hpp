/*
  ATLAN C++ Library: String.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
 
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__STRING_HPP__
#define __ACL__STRING_HPP__
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <stdio.h>

namespace acl{

class String : public std::string{
public:
	String() {}
	String(const char *buf) : std::string(buf) {}
	String(char *buf) : std::string(buf) {}
	String(const std::string& s) : std::string(s) {}
	String(const String& s) : std::string(s) {}
	String(const int& n){
			char cache[16];
#ifdef WIN32
			sprintf_s(cache,"%d",n);
#else
			sprintf(cache,"%d",n);
#endif
			*this = cache;
        }
    String(const unsigned int& n){
                char cache[16];
#ifdef WIN32
                sprintf_s(cache,"%d",n);
#else
				sprintf(cache,"%d",n);
#endif
                *this = cache;
        }
	String(const long unsigned int& n){
                char cache[32];
#ifdef WIN32
                sprintf_s(cache,"%d",n);
#else
                                sprintf(cache,"%lu",n);
#endif
                *this = cache;
        }
#ifdef WIN32
	String(const LPCSTR& lpstr){
		std::wstring wstr = lpstr;
		*this = std::string(wstr.begin(), wstr.end());
	}
#endif

	template<class T>
	String(const T& t){
		std::stringstream ss(t);
                *this = ss.str();
	}

	std::vector<std::string> separate(const String& separator) const{
		std::vector<std::string> v;
		size_t pos,beg = 0;

		for(;;){
			pos = this->find(separator,beg);
			v.push_back(this->substr(beg, pos-beg));
			beg = pos + separator.length();
			if(pos == std::string::npos) break;
		}

		return v;
	}

	std::vector<acl::String> separate_(const String& separator) const{
		std::vector<acl::String> v;
		size_t pos,beg = 0;

		for(;;){
			pos = this->find(separator,beg);
			v.push_back(this->substr(beg, pos-beg));
			beg = pos + separator.length();
			if(pos == std::string::npos) break;
		}

		return v;
	}

	std::vector<std::string> separate(const char& separator) const{
		std::vector<std::string> v;
		size_t pos,beg = 0;

		for(;;){
			pos = this->find(separator,beg);
			for(;isEscapedChar(pos) && pos!=std::string::npos;){
				pos = this->find(separator,++pos);
			}
			v.push_back(this->substr(beg, pos-beg));
			beg = pos + 1;
			if(pos == std::string::npos) break;
		}

		return v;
	}

	void toLower(){
		for(size_t i=0;i<this->length();i++){
			if(isupper((*this)[i])){
				(*this)[i] = tolower((*this)[i]);
			}
		}
	}

	void toUpper(){
		for(size_t i=0;i<this->length();i++){
			if(islower((*this)[i])){
				(*this)[i] = toupper((*this)[i]);
			}
		}
	}

	template<class T>
	T to() const{
		std::stringstream ss(*this);
		T t;
		ss >> t;
		return t;
	}

	void replace(const String& from, const String& to){
		size_t beg = 0, pos = std::string::npos;
		for(;;){
			pos = this->find(from,beg);
			if(pos == std::string::npos) break;
			*this = this->substr(0,pos) + to + this->substr(pos+from.length());
			beg = pos + to.length();
		}
	}
	
	void removeChars(const std::string& chars){
		std::string s;
		
		for(size_t i=0; i<chars.length(); i++){
			removeChar(chars[i]);
		}
	}
	
	void removeChar(char c){
		std::string s;
		
		for(size_t i=0; i<this->length(); i++){
			if(this->at(i) == c && !isEscapedChar(i)){
				continue;
			}else{
				s += this->at(i);
			}
		}
		
		this->swap(s);
	}
	
	void remove(const std::string& target){
		size_t beg = 0, pos = std::string::npos;
		for(;;){
			pos = this->find(target,beg);
			if(pos == std::string::npos) break;
			*this = this->substr(0,pos) + this->substr(pos+target.length());
			beg = pos;
		}
	}
	
	void removeBetween(const std::string& sBeg, const std::string& sEnd){
		size_t beg = 0, pos = std::string::npos;
		for(;;){
			beg = this->find(sBeg,beg);
			if(beg == std::string::npos) break;
			pos = this->find(sEnd,beg);
			if(pos == std::string::npos){
				beg++;
				continue;
			}
			*this = this->substr(0,beg) + this->substr(pos+sEnd.length());
			beg = pos + sEnd.length();
		}
	}
	
	std::vector<std::string> toCharactor() const{
		std::vector<std::string> v;
		size_t nMax = this->length()-1;
		
		for(size_t i=0;i<this->length();){
			if(((*this)[i] & 0x80) == 0x00){
				//英文
				v.push_back((*this).substr(i++,1));
			}else if(((*this)[i] & 0xE0) == 0xC0 && i<nMax){
				//少量中文
				v.push_back((*this).substr(i,2));
				i+=2;
			}else if(((*this)[i] & 0xF0) == 0xE0 && i<nMax-1){
				//大多数中文
				v.push_back((*this).substr(i,3));
				i+=3;
			}else{
				v.clear();
				return v;
			}
		}
		
		return v;
	}
	
	// toCharactor()的改进，考虑拉丁字符
	// 我是Robosay机器人  --->  '我'+'是'+'Robosay'+'机'+'器'+'人'
	std::vector<std::string> toCharactorWithLatin() const{
		std::vector<std::string> v;
		acl::String latinWord = "";
		
		size_t i=0;
		while(i<this->length()){
			if((*this)[i]<225 && (*this)[i]>0){              // Latin文字
				latinWord = latinWord + (*this).substr(i,1);
				i++;
				while((*this)[i]<225 && (*this)[i]>0){
					latinWord = latinWord + (*this).substr(i,1);
					i++;
				}
				latinWord.toUpper();
				v.push_back(latinWord);
			}else{                                          // 中文切分
				v.push_back((*this).substr(i,3));
				i = i + 3;
			}
		}
		
		return v; 
	}
	
	std::set<std::string> toCharactorSet() const{
		std::set<std::string> se;
		size_t nMax = this->length()-1;
		
		for(size_t i=0;i<this->length();){
			if(((*this)[i] & 0x80) == 0x00){
				se.insert((*this).substr(i++,1));
			}else if(((*this)[i] & 0xE0) == 0xC0 && i<nMax){
				se.insert((*this).substr(i,2));
				i+=2;
			}else if(((*this)[i] & 0xF0) == 0xE0 && i<nMax-1){
				se.insert((*this).substr(i,3));
				i+=3;
			}else{
				se.clear();
				return se;
			}
		}
		
		return se;
	}
	
	std::string toHalf() const{
		size_t i = 0;
		std::string strRet;

		while (i < this->length()) {
			char cOneChar = this->at(i);
			//from full-width ! to _
			if ((cOneChar == -17) && (this->at(i+1) == -68)) {
				if ((this->at(i+2) >= -127) && (this->at(i+2) <= -65)) {
					char cTempChar = this->at(i+2) + 160;
					strRet.insert(strRet.end(), cTempChar);
					i = i + 3;
				}else{
					strRet.insert(strRet.end(), this->at(i));
					strRet.insert(strRet.end(), this->at(i+1));
					strRet.insert(strRet.end(), this->at(i+2));
					i = i + 3;
				}
			}
			//from full-width a to ~
			else if ((cOneChar == -17) && (this->at(i+1) == -67)) {
				if ((this->at(i+2) >= -127) && (this->at(i+2) <= -98)) {
					char cTempChar = this->at(i+2) + 224;
					strRet.insert(strRet.end(), cTempChar);
					i = i + 3;
				}else{
					strRet.insert(strRet.end(), this->at(i));
					strRet.insert(strRet.end(), this->at(i+1));
					strRet.insert(strRet.end(), this->at(i+2));
					i = i + 3;
				}
			}
			//full-width space
			else if((cOneChar == -29) && (this->at(i+1) == -128) && (this->at(i+2) == -128)){
				char cTempChar = 32;
				strRet.insert(strRet.end(), cTempChar);
				i = i + 3;
			}
			/*full-width \*/
			else if ((cOneChar == -17) && (this->at(i+1) == -65) && (this->at(i+2) == -91)) {
				char cTempChar = 92;
				strRet.insert(strRet.end(), cTempChar);
				i = i + 3;
			}
			//full-width '
			else if((cOneChar==(char)0xe2) && (this->at(i+1)==(char)0x80) && (this->at(i+2)==(char)0x99)){
				char cTempChar = (char)0x27;
				strRet.insert(strRet.end(), cTempChar);
				i = i + 3;
			//full-width "
			}else if ((cOneChar==(char)0xe2) && (this->at(i+1)==(char)0x80) && (this->at(i+2)==(char)0x9d)){
				char cTempChar = (char)0x22;
				strRet.insert(strRet.end(), cTempChar);
				i = i + 3;
			}else {
				if((cOneChar& 0xF0) == 0xE0) {
					strRet.insert(strRet.end(), this->at(i));
					strRet.insert(strRet.end(), this->at(i+1));
					strRet.insert(strRet.end(), this->at(i+2));
					i = i + 3;
				}else if((cOneChar & 0xE0) == 0xC0){
					strRet.insert(strRet.end(), this->at(i));
					strRet.insert(strRet.end(), this->at(i+1));
					i = i + 2;
				}else{
					strRet.insert(strRet.end(), this->at(i));
					i = i + 1;
				}
			}
		}
		return strRet;
	}
	
	std::string getCode() const{
		std::stringstream ss;
		
		ss<<"{";
		for(size_t i=0; i<this->length(); i++){
			ss << "(char)0x" << std::hex << ((int)this->at(i) & 0x000000ff) << ",";
		}
		ss << "(char)0x00}";
		
		return ss.str();
	}
	
	std::vector<String> extractAlphaNumberString() const{
		std::vector<String> res;
		String curStr = "";
		for(size_t i=0;i<this->length();i++){
			bool isfind = false;
			while (i<this->length() && (('A' <= this->at(i) && this->at(i) <= 'Z') || ('a' <= this->at(i) && this->at(i) <= 'z') 
				|| ('0' <= this->at(i) && this->at(i) <= '9') || this->at(i) == '#' || this->at(i) == '*')){
					isfind = true;
					curStr += this->at(i++);
			}
			if(isfind){
				res.push_back(curStr);
				curStr = "";
			}
		}
		return res;
	}
	inline bool isEscapedChar(const int& pos) const{
		bool ret = false;
		
		for(int i=pos-1;i>-1;i--){
			if((*this)[i] == '\\') switchBoolTag(ret);
			else break;
		}
		
		return ret;
	}
	
	void toArabic(){
		String s = "零一二三四五六七八九";
		std::vector<std::string> v = s.toCharactor();
		
		for(size_t i=0; i<v.size(); i++){
			String sTar(i);
			this->replace(v[i],sTar);
		}
	}
	void strip(){
		int beg = 0;
		int end = (int)this->length();
		
		//find begin position
		while(beg < end && ( (*this)[beg] == '\t' || (*this)[beg] == ' ' ) ){
			beg++;
		}
		
		//find end position
		for(int i=end-1; i>-1; i--){
			if( (*this)[i] == '\t' || (*this)[i] == ' ' || (*this)[i] == '\r' || (*this)[i] == '\n' ){
				end = i;
			}else{
				break;
			}
		}
		
		(*this) = this->substr(beg,end-beg);
	}

	void cutSentenceFromAnswer(const std::set<std::string>& setSeperate,std::vector<std::string>& vecReturn) const
	{
		std::string v;
		int end = 0, dispace = 0;
		int i =0;
		int nMax = (int)this->length()-1;	

		for( i=0;i<(int)this->length();){
			if(((*this)[i] & 0x80) == 0x00){
				dispace = 1;
				v = this->substr(i,1);
			}else if((((*this)[i] & 0xE0) == 0xC0) && i<nMax){
				dispace = 2;
				v = this->substr(i,2);
			}else if((((*this)[i] & 0xF0) == 0xE0) && i<nMax-1){
				v = this->substr(i,3);
				dispace = 3;;
			}else{
				vecReturn.clear();
				return ;
			}
			std::set<std::string>::const_iterator it1; 
			it1 = setSeperate.find(v);  
			if(it1 != setSeperate.end())  
			{
				std::string s = this->substr(end,i-end);
				s = removeTabSpaceEnterAtBegin(s);
				if(!s.empty())
				{
					vecReturn.push_back(s);
				}
				std::cout<<"分词结果:"<<s<<std::endl;
				end = i+dispace;
			}
			i += dispace;
		}
		
		if(end<i)
		{
			std::string s = this->substr(end,i-end);
			s = removeTabSpaceEnterAtBegin(s);
			if(!s.empty())	
			{
				vecReturn.push_back(s);
			}
			std::cout<<"分词结果:"<<s<<std::endl;
		}
		return ;
	}

static std::string convertNumberCharactor(const std::string& src){
	    acl::String Src(src);
        std::vector<std::string> vChar = Src.toCharactor();
        std::string result;

        std::vector<std::string>::reverse_iterator  caculateIter = vChar.rbegin(),tmpIter;
        for(; caculateIter!=vChar.rend(); ++caculateIter){
            if(*caculateIter == "零"){
                *caculateIter = "0";
            }else if(*caculateIter == "一"){
                *caculateIter = "1";
            }else if(*caculateIter == "二"){
                *caculateIter = "2";
            }else if(*caculateIter == "三"){
                *caculateIter = "3";
            }else if(*caculateIter == "四"){
                *caculateIter = "4";
            }else if(*caculateIter == "五"){
                *caculateIter = "5";
            }else if(*caculateIter == "六"){
                *caculateIter = "6";
            }else if(*caculateIter == "七"){
                *caculateIter = "7";
            }else if(*caculateIter == "八"){
                *caculateIter = "8";
            }else if(*caculateIter == "九"){
                *caculateIter = "9";
            }else if(*caculateIter == "十"){
                std::string sNumber = "一二三四五六七八九123456789";
                bool bBeforeIsNumber = false, bAfterIsNumber = false;
                // 判断“十”的前一个汉字是否是其它数字
                tmpIter = caculateIter;
                tmpIter++;
                if(tmpIter!=vChar.rend() && sNumber.find(*tmpIter) != std::string::npos){
                    bBeforeIsNumber = true;
                }
                // 判断“十”的后一个汉字是否是其它数字
                if(caculateIter!=vChar.rbegin()){
                    tmpIter = caculateIter;
                    tmpIter--;
                    if(sNumber.find(*tmpIter) != std::string::npos){
                        bAfterIsNumber = true;
                    }
                }
                // 根据“十”的前后数字判断“十”的数字表达
                if(bBeforeIsNumber==false && bAfterIsNumber==false){// 二月十日
                    *caculateIter = "10";
                }else if(bBeforeIsNumber==true && bAfterIsNumber==false){
                    *caculateIter = "0";
                }else if(bBeforeIsNumber==false && bAfterIsNumber==true){
                    *caculateIter = "1";
                }else{
                    *caculateIter = "";
                }
            }else if((*caculateIter).length()==1 && '0'<(*caculateIter)[0] && (*caculateIter)[0]<'9'){
                continue;
            }
        }
        
        for(size_t i=0;i<vChar.size();i++){
            result+=vChar[i];
        }
        
        return result;
    }	
private:
	inline void switchBoolTag(bool& tag) const{
		tag == true ? tag = false : tag = true;
	}

	std::string removeTabSpaceEnterAtBegin(const std::string& dest) const
	{
	    int i,length;
		bool bFind = false;
	    length = dest.length();

		if(dest.empty()){
			return "";
		}
	    int pos = 0;
	    for(i=0; i<length; i++) {
	        if(dest[i] == '\0' || dest[i] == '\r' || dest[i] == '\n' || dest[i] == '\t') {
	            pos = i;
				bFind = true;
	        } else {
	            break;
	        }
	    }

		if(bFind){
			if(pos==length-1)
			{
				return "";
			}
			return dest.substr(pos+1);
		}
		else
		{
			return dest;
		}
	}	
};

}//namespace acl

#endif //__ACL__STRING_HPP__
