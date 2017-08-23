/*
  ATLAN C++ Library: StringMap.hpp
  A lite map, using string to record a very small map.
  
  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__STRING_MAP_H__
#define __ACL__STRING_MAP_H__
#include "String.hpp"
#include <map>
#include <set>

namespace acl{

class StringMap : public std::map<std::string,std::string>
{
public:
	StringMap(){}
	
	StringMap(const std::string& sMap){
		fromStr(sMap);
	}
	
	void init(const std::set<std::string>& se){
		std::set<std::string>::const_iterator iter;
		for(iter=se.begin(); iter!=se.end(); ++iter){
			this->insert(std::make_pair(*iter,""));
		}
	}
	
	void init(const std::map<std::string,std::string>& m){
		std::map<std::string,std::string>::const_iterator iter;
		for(iter=m.begin(); iter!=m.end(); ++iter){
			this->insert(std::make_pair(iter->first,iter->second));
		}
	}
	
	void update(const StringMap& mVar){
		std::map<std::string,std::string>::const_iterator iter;
		std::map<std::string,std::string>::iterator iter1;
		
		for(iter=mVar.begin(); iter!=mVar.end(); ++iter){
			iter1 = this->find(iter->first);
			if(iter1 != this->end()){
				iter1->second = iter->second;
			}
		}
	}
	
	void update(const std::map<std::string,std::string>& mVar){
		std::map<std::string,std::string>::const_iterator iter;
		std::map<std::string,std::string>::iterator iter1;
		
		for(iter=mVar.begin(); iter!=mVar.end(); ++iter){
			iter1 = this->find(iter->first);
			if(iter1 != this->end()){
				iter1->second = iter->second;
			}
		}
	}
	
	void incrementUpdate(const std::map<std::string,std::string>& mVar){
		std::map<std::string,std::string>::const_iterator iter;
		std::map<std::string,std::string>::iterator iter1;
		
		for(iter=mVar.begin(); iter!=mVar.end(); ++iter){
			iter1 = this->find(iter->first);
			if(iter1 != this->end()){
				if(iter1->second.empty()){ // 若变量存在且为空，则写入
					iter1->second = iter->second;
				}else{ // 若变量存在且不为空，则给Key增量，并追加写入
					std::string sKey = generateNewKey(iter1->first);
					this->insert(std::make_pair(sKey,iter->second));
				}
			}else{// 若变量不存在，则追加写入
				this->insert(std::make_pair(iter->first,iter->second));
			}
		}
	}
	
	bool hasKey(const std::string& sKey) const{
		std::map<std::string,std::string>::const_iterator iter = this->find(sKey);
		return iter == this->end() ? false : true;
	}
	
	bool isEmpty(const std::string& sKey) const{
		std::map<std::string,std::string>::const_iterator iter = this->find(sKey);
		if(iter == this->end()){
			return false;
		}else{
			return iter->second.empty() ? true : false;
		}
	}
	
	bool hasValue(const std::string& sKey) const{
		std::map<std::string,std::string>::const_iterator iter = this->find(sKey);
		if(iter == this->end()){
			return false;
		}else{
			return iter->second.empty() ? false : true;
		}
	}
	
	void removeKey(const std::string& sKey) {
		std::map<std::string,std::string>::iterator iter = this->find(sKey);
		if(iter != this->end()){
			this->erase(iter);
		}
	}
	
	std::string getValue(const std::string& sKey) const{
		std::map<std::string,std::string>::const_iterator iter = this->find(sKey);
		return iter == this->end() ? "Null" : iter->second;
	}
	
	void reset(){
		std::map<std::string,std::string>::iterator iter;
		for(iter=this->begin(); iter!=this->end(); ++iter){
			iter->second = "";
		}
	}
	
	std::string toStr() const{
		std::string sRet;
		std::map<std::string,std::string>::const_iterator iter;
		
		sRet += "{";
		for(iter=this->begin(); iter!=this->end(); ++iter){
			sRet += "[" + iter->first + "-" + iter->second + "]";
		}
		sRet += "}";
		
		return sRet;
	}
	
	bool fromStr(const std::string& s){
		if(s.empty() || s == "{}") return true;
		if(s.size()<4 || s.at(0)!='{' || s.at(s.length()-1)!='}') return false;
		if(s.at(1)!='[' || s.at(s.length()-2)!=']') return false;

		acl::String str = s.substr(2,s.length()-4);
		std::vector<std::string> v = str.separate("][");
		std::map<std::string,std::string>::iterator iter;
		for(size_t i=0; i<v.size(); i++) {
			const std::string& sVar = v.at(i);
			int nSep = sVar.find('-');
			if(nSep == std::string::npos) return false;
			std::string sKey = sVar.substr(0,nSep);
			std::string sVal = sVar.substr(nSep+1);
			iter = this->find(sKey);
			if(iter == this->end()){
				this->insert(std::make_pair(sKey,sVal));
			}else{
				iter->second = sVal;
			}
		}
		
		return true;
	}
	
	int getValueCount() const{
		std::map<std::string,std::string>::const_iterator iter;
		int nCount = 0;
		
		for(iter=this->begin(); iter!=this->end(); ++iter){
			if(!iter->second.empty()){
				nCount++;
			}
		}
		
		return nCount;
	}
private:
	std::string generateNewKey(std::string sKey) const{
		size_t nTail = sKey.length() - 1;
		
		if('1' <= sKey[nTail] && sKey[nTail] <= '9'){
			char cIndex = sKey[nTail];
			cIndex++;
			sKey[nTail] = cIndex;
		}else{
			char cIndex = '1';
			sKey += cIndex;
		}
		
		return sKey;
	}
};

}//namespace acl

#endif //__ACL__STRING_MAP_H__
