/*
  ATLAN C++ Library: Formula.hpp
  A multi-purpose parser of the four arithmetic operations.
  
  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__FORMULA_HPP__
#define __ACL__FORMULA_HPP__
#include "String.hpp"
#include "Definations.hpp"

namespace acl{

const char lp1 = '+'; //lp: low  priority
const char lp2 = '-';
const char hp1 = '*'; //hp: high priority
const char hp2 = '/';

class Formula : public String{
public:
	template<class T>
	Formula(const T& t) : String(t){}
	bool expand(){
		std::vector<String> cache = divide();
		
		for(size_t i=0; i<cache.size(); i++){
			
		}
		
		return true;
	}
	
private:
	inline std::vector<String> divide() const{
		String factor;
		std::vector<String> cache;
		
		for(size_t i=0; i<this->size(); i++){
			if( isOperator(i) ){
				if(!factor.empty()) cache.push_back(factor);
				factor = "";
				cache.push_back(String(this->at(i)));
			}else if( isBracket(i) ){
				if(!factor.empty()) cache.push_back(factor);
				factor = "";
				cache.push_back(String(this->at(i)));
			}else{
				factor += this->at(i);
			}
		}
		if(!factor.empty()) cache.push_back(factor);
		
		return cache;
	}
	
	inline bool isOperator(size_t pos) const{
		char ch = this->at(pos);
		return ((ch == lp1 || ch == lp2 || ch == hp1 || ch == hp2) && !isEscapedChar(pos)) ? true : false;
	}
	
	inline bool isLowPriority(size_t pos) const{
		char ch = this->at(pos);
		return ( ch == lp1 || ch == lp2 ) ? true : false;
	}
	
	inline bool isBracket(size_t pos) const{
		char ch = this->at(pos);
		return ((ch == '(' || ch == ')') && !isEscapedChar(pos)) ? true : false;
	}
};

}//namespace acl

#endif //__ACL__FORMULA_HPP__
