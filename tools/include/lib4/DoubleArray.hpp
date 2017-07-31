/*
  ATLAN C++ Library: DoubleArray.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__DOUBLE_ARRAY_HPP__
#define __ACL__DOUBLE_ARRAY_HPP__
#include "Darts.h"
#include "File.hpp"
#include <iostream>
#include <map>

namespace acl{

const unsigned int DOUBLE_ARRAY_RESULT_MAX_SIZE = 20;

template<class T>
class DoubleArray{
public:
	DoubleArray(){}
	
	bool init(const std::map<std::string, T>& m){
		const size_t size = m.size();
		std::vector<const char*> keyArray(size);
		std::vector<int> valIndex(size);
		int index = 0;
		typename std::map<std::string, T>::const_iterator iter;
		
		valueArray.resize(size);
		for(iter=m.begin(); iter!=m.end(); ++iter,index++){
			keyArray[index] = iter->first.c_str();
			valueArray[index] = iter->second;
			valIndex[index] = index;
		}
		
		if(dArray.build(size, &keyArray[0], 0, &valIndex[0], 0) != 0){
			return false;
		}
		
		return true;
	}
	
	bool init(const std::string& filename, const std::string& separator = "\t"){
		File file;
		std::vector<const char*> keyArray;
		std::vector<int> valIndex;
		
		if(file.read(filename) == false){
			return false;
		}
		
		for(size_t i=0; i<file.size(); i++){
			if(file[i].empty()) continue;
			const String& line = dynamic_cast<String&>(file[i]);
			std::vector<String> v = line.separate(separator);
			if(v.size() != 2) return false;
			keyArray.push_back(v[0].c_str());
			valIndex.push_back(valueArray.size());
			valueArray.push_back(v[1]);
		}
		
		if(dArray.build(keyArray.size(), &keyArray[0], 0, &valIndex[0], 0) != 0){
			return false;
		}
		
		return true;
	}
	
	const T& find(const std::string& s) const{
		const size_t length = s.length();
		acl::Darts::DoubleArray::result_pair_type result[DOUBLE_ARRAY_RESULT_MAX_SIZE];
		
		size_t resultSize = dArray.commonPrefixSearch(s.c_str(),result,length);
		if( resultSize > 0 ){
			if(result[--resultSize].length == length){
				return valueArray[result[resultSize].value];
			}
		}
		
		return nullValue;
	}

public:
	const T nullValue;
	
private:
	std::vector<T> valueArray;
	acl::Darts::DoubleArray dArray;
};

}//namespace acl

#endif //__ACL__DOUBLE_ARRAY_HPP__
