/*
  ATLAN C++ Library: Vector.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__VECTOR_HPP__
#define __ACL__VECTOR_HPP__
#include <iostream>
#include <vector>
#include <set>
#include "Type.hpp"

namespace acl{

template<class T>
class Vector : public std::vector<T>{
public:
	Vector() {}
	Vector(const std::vector<T>& v){
		for(size_t i=0; i<v.size(); i++){
			this->push_back(v.at(i));
		}
	}
        template<class U>
        Vector(const std::vector<U>& v){
		for(size_t i=0; i<v.size(); i++){
			this->push_back(Type::convert<U,T>(v.at(i)));
		}
	}
	
	bool hasEmptyElement() const{
		for(size_t i=0; i<this->size(); i++){
			if(this->at(i).empty()){
				return true;
			}
		}
		return false;
	}
	
	std::string combine(const char* separator = " ") const{
		if(this->empty()) return "";
		
		std::string s;
		s += (*this)[0];
		for(size_t i=1; i<this->size(); i++){
			s += separator + (*this)[i];
		}
		return s;
	}
	
	std::string combine(size_t beg, size_t end, const char* separator = " ") const{
		if(this->empty()) return "";
		
		std::string s;
		s += (*this)[beg];
		for(size_t i=beg+1; i<=end; i++){
			s += separator + (*this)[i];
		}
		return s;
	}
	
	size_t find(const T& t) const{
		for(size_t i=0; i<this->size(); i++){
			if(t == this->at(i)){
				return i;
			}
		}
		return std::string::npos;
	}
	
	void adjacentUnique(){
		T* last = NULL;
		
		for(typename std::vector<T>::iterator i=this->begin();i!=this->end();++i){
			if(last != NULL){
				if(*last == *i){
					erase(i);
					i--;
				}
			}
			last = &(*i);
		}
	}
	
        std::set<std::string> toSet() const{
            std::set<std::string> se;

            for(size_t i=0; i<this->size(); i++){
                se.insert(this->at(i));
            }

            return se;
        }

	void dump(std::ostream& ofs = std::cout, const char* separator = ",") const{
		ofs<<"{"<<this->combine(separator)<<"}"<<std::endl;
	}
};

}//namespace acl

#endif //__ACL__VECTOR_HPP__
