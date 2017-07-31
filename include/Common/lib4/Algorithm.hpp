/*
  ATLAN C++ Library: Algorithm.hpp
  
  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__ALGORITHM_HPP__
#define __ACL__ALGORITHM_HPP__
#include "String.hpp"
#include "Definations.hpp"

namespace acl{

class FormulaCache : public std::vector<std::string>{
public:
	FormulaCache(size_t beg = std::string::npos, size_t end = std::string::npos){
		m_beg = beg;
		m_end = end;
	}
	void clear(){
		m_beg = std::string::npos;
		m_end = std::string::npos;
		std::vector<std::string>::clear();
	}
	void dump(std::ostream& ofs = std::cout) const{
		ofs<<"["<<m_beg<<","<<m_end<<"]:(";
		for(size_t i=0; i<this->size(); i++){
			if(i != 0) ofs<<"/";
			ofs<<(*this)[i];
		}
		ofs<<")";
	}
	void setStartPosition(size_t beg){
		m_beg = beg;
	}
	size_t getStartPosition() const{
		return m_beg;
	}
	void setEndPosition(size_t end){
		m_end = end;
	}
	size_t getEndPosition() const{
		return m_end;
	}
private:
	size_t m_beg;
	size_t m_end;
};

class FormulaElementIndexCache : public std::vector<size_t>{
public:
	bool update(){
		bool carry_flag = true;
		for(int i=this->size()-1; i>-1; i--){
			if(carry_flag == true){
				(*this)[i]++;
				carry_flag = false;
			}
			if((*this)[i] > m_FormulaElementMax[i]){
				if(i==0) return false;
				(*this)[i] = 0;
				carry_flag = true;
			}
		}
		return true;
	}
	void push_back(const size_t& num){
		std::vector<size_t>::push_back(0);
		m_FormulaElementMax.push_back(num-1);
	}
	void dump(std::ostream& ofs = std::cout) const{
		for(size_t i=0; i<this->size(); i++){
			ofs<<i<<":"<<(*this)[i]<<"  ";
		}
	}
private:
	std::vector<size_t> m_FormulaElementMax;
};

class VFormulaCache : public std::vector<FormulaCache>{
public:
	std::vector<std::string> combinedFormulaWords(const std::string& s){
		std::vector<std::string> result;
		std::vector<std::string> normalStringCache = makeNormalStringCache(s);
		FormulaElementIndexCache indexCache;
		
		for(size_t i=0; i<this->size(); i++){
			indexCache.push_back((*this)[i].size());
		}
		
		do{
			std::string cache;
			for(size_t j=0; j<this->size(); j++){
				cache += normalStringCache[j] + (*this)[j][indexCache[j]];
			}
			result.push_back(cache+normalStringCache[this->size()]);
		}while(indexCache.update() == true);
		
		return result;
	}

	template<class CallBackFuncType>
	void combinedFormulaWords(const std::string& s, CallBackFuncType callBackFunc){
		//std::vector<std::string> result;
		std::vector<std::string> normalStringCache = makeNormalStringCache(s);
		FormulaElementIndexCache indexCache;
		
		for(size_t i=0; i<this->size(); i++){
			indexCache.push_back((*this)[i].size());
		}
		
		do{
			std::string cache;
			for(size_t j=0; j<this->size(); j++){
				cache += normalStringCache[j] + (*this)[j][indexCache[j]];
			}
			callBackFunc(cache+normalStringCache[this->size()]);
			//result.push_back(cache+normalStringCache[this->size()]);
		}while(indexCache.update() == true);
		
		//return result;
	}
	
	bool readFormulaWords(const std::string& s){
		FormulaCache formulaCache;
		std::string wordCache;
		bool inBracket = false;
		
		for(size_t i=0; i<s.size(); i++){
			if( s[i] == '{' ){
				formulaCache.clear();
				wordCache = "";
				formulaCache.setStartPosition(i);
				inBracket = true;
			}else if( s[i] == '}' && inBracket == true){
				formulaCache.setEndPosition(i);
				formulaCache.push_back(wordCache);
				wordCache = "";
				this->push_back(formulaCache);
				inBracket = false;
			}else if( s[i] == '/' && inBracket == true ){
				formulaCache.push_back(wordCache);
				wordCache = "";
			}else if( inBracket == true ){
				wordCache += s[i];
			}
		}
		
		return this->empty() ? false : true;
	}
	void dump(std::ostream& ofs = std::cout) const{
		for(size_t i=0; i<this->size(); i++){
			(*this)[i].dump(ofs);
			ofs<<std::endl;
		}
	}
private:
	std::vector<std::string> makeNormalStringCache(const std::string& s) const{
		const size_t size = this->size();
		std::vector<std::string> normalStringCache;
		
		normalStringCache.push_back( s.substr(0,(*this)[0].getStartPosition()) );
		for(size_t i=1; i<size; i++){
			normalStringCache.push_back( s.substr((*this)[i-1].getEndPosition()+1,(*this)[i].getStartPosition()-(*this)[i-1].getEndPosition()-1) );
		}
		normalStringCache.push_back( s.substr((*this)[size-1].getEndPosition()+1) );
		
		return normalStringCache;
	}
};

class Algorithm{
public:
	//expand "{A/B/C}D{E/}" to "ADE","BDE","CDE","AD","BD","CD"
	static std::vector<std::string> permutationAndCombination(const String& input){
		VFormulaCache vFormulaCache;
		std::vector<std::string> result;
		
		if(vFormulaCache.readFormulaWords(input) == false){
			result.push_back(input);
		}else{
			result = vFormulaCache.combinedFormulaWords(input);
		}
		
		return result;
	}

	template<class CallBackFuncType>
	static void permutationAndCombination(const String& input, CallBackFuncType callBackFunc){
		VFormulaCache vFormulaCache;
		
		if(vFormulaCache.readFormulaWords(input) == false){
			callBackFunc(input);
		}else{
			vFormulaCache.combinedFormulaWords(input,callBackFunc);
		}
	}
};

}//namespace acl

#endif //__ACL__ALGORITHM_HPP__
