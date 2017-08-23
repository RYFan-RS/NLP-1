/*
  ATLAN C++ Library: Line.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__LINE_HPP__
#define __ACL__LINE_HPP__
#include "String.hpp"
#include "Vector.hpp"

namespace acl{

class Line : public String{
public:
	Line() {}
	Line(const char *buf) : String(buf) {}
	Line(char *buf) : String(buf) {}
	Line(const std::string& s) : String(s) {}
	
	void removeBackComment(){
		size_t pos = this->find("//");
		if(pos != std::string::npos){
			*this = this->substr(0,pos);
		}
	}
		
	void stripBack(){
		int beg = 0;
		int end = (int)this->length();
		
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
	
	bool isBeginWith(const std::string& s) const{
		if(s.empty() || s.length() > this->length()) return false;
		
		for(size_t i=0; i<s.length(); i++){
			if( (*this)[i] != s[i] ) return false;
		}
		
		return true;
	}
	
	bool isBeginWith(const Vector<String>& v) const{
		for(size_t i=0; i<v.size(); i++){
			if(isBeginWith(v[i])) return true;
		}
		
		return false;
	}
	
	void removeUTF8BOM(){
		if(this->find("\xef\xbb\xbf") == 0){
			*this = this->substr(3);
		}
	}
};

}//namespace acl

#endif //__ACL__LINE_HPP__
