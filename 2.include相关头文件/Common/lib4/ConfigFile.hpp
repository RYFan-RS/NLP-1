/*
  ATLAN C++ Library: ConfigFile.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__CONFIG_FILE_HPP__
#define __ACL__CONFIG_FILE_HPP__
#include <map>
#include "File.hpp"
#include "System.hpp"

namespace acl{

class ConfigFile : public File{
public:
	ConfigFile() {}
	
	bool read(const String& filename){
		std::pair<String,String> kv;
		
		if( System::isFileExist(filename) == false ){
			return false;
		}
		
		if(!File::read(filename)){
			return false;
		}
		
		for(size_t i=0; i<this->size(); i++){
			if(!(*this)[i].isBeginWith(commentMarks) && !(*this)[i].empty()){
				kv = getKeyAndValue((*this)[i]);
				paramMap.insert(kv);
				paramIndexMap.insert(std::pair<String,unsigned int>(kv.first,i));
			}
		}
		
		return true;
	}
	
	const String& getValue(const String& key) const{
		std::map<String,String>::const_iterator iter = paramMap.find(key);
		return iter == paramMap.end() ? nullValue : iter->second;
	}
	
	const std::map<String,String>& getParamMap() const{
		return paramMap;
	}
	
	bool update(const String& key, const String& value){
		std::map<String,String>::iterator iter = paramMap.find(key);
		
		if(iter == paramMap.end()) return false;
		
		iter->second = value;
		(*this)[paramIndexMap[key]] = key + " = " + value;
		
		return true;
	}
	
	void setCommentMarks(const String& marks){
		commentMarks = marks.separate(',');
	}
	
	const Vector<String>& getCommentMarks() const{
		return commentMarks;
	}
	
	void clearCommentMarks(){
		commentMarks.clear();
	}
	
	void dumpParam(std::ostream& ofs = std::cout, const char* separator = "-->") const{
		std::map<String,String>::const_iterator iter;
		for(iter = paramMap.begin(); iter != paramMap.end(); ++iter){
			ofs<<iter->first<<separator<<iter->second<<std::endl;
		}
	}
private:
	std::pair<String,String> getKeyAndValue(const Line& line) const{
		size_t pos = line.find("=");
		Line key = line.substr(0,pos);
		Line value = line.substr(pos+1);
		key.strip();
		value.strip();
		return std::pair<String,String>(key,value);
	}
	
private:
	const String nullValue;
	Vector<String> commentMarks;
	std::map<String,String> paramMap;
	std::map<String,unsigned int> paramIndexMap;
};

}//namespace acl

#endif //__ACL__CONFIG_FILE_HPP__
