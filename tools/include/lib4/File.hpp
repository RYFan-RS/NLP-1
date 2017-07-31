/*
  ATLAN C++ Library: File.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__FILE_HPP__
#define __ACL__FILE_HPP__
#include <fstream>
#include <set>
#include <stdlib.h>
#include "Line.hpp"

namespace acl{

class File : public std::vector<Line>{
public:
	File() {}
	File(const std::string& filename){
		if(!read(filename)){
			exit(1);
		}
	}
	File(const std::vector<std::string>& v){
		for(size_t i=0; i<v.size(); i++){
			this->push_back(v[i]);
		}
	}

	bool read(const std::string& filename){
		Line line;
		
		std::fstream ifp(filename.c_str(),std::ios::in);
		if(!ifp){
			std::cerr<<__FILE__<<":"<<__LINE__<<"=>Can't open file `"<<filename<<"'"<<std::endl;
			return false;
		}
		
		while(!ifp.eof()){
			std::getline(ifp,line);
			if(this->empty()){
				line.removeUTF8BOM();
			}
			line.stripBack();
			if(!line.isBeginWith(commentMarks) && !line.empty()){
				this->push_back(line);
			}
		}
		
		ifp.close();
		
		return true;
	}
	
	std::vector<std::vector<std::string> > separate(const std::string& separatorLine) const{
		std::vector<std::vector<std::string> > result;
		std::vector<std::string> cache;
		
		for(size_t i=0; i<this->size(); i++){
			if(this->at(i) == separatorLine){
				if(cache.empty()){
					continue;
				}else{
					result.push_back(cache);
					cache.clear();
				}
			}else{
				cache.push_back(this->at(i));
			}
		}
		
		if(!cache.empty() && !cache[0].empty()){
			result.push_back(cache);
		}
		
		return result;
	}
	
	std::set<std::string> toSet() const{
		std::set<std::string> result;
		
		for(size_t i=0; i<this->size(); i++){
			if(this->at(i).empty()) continue;
			result.insert(this->at(i));
		}
		
		return result;
	}
	
	std::vector<std::string> toVector() const{
		std::vector<std::string> vRet;
		for(size_t i=0; i<this->size(); i++){
			if(this->at(i).empty()) continue;
			vRet.push_back(this->at(i));
		}
		return vRet;
	}
	
#ifndef WIN32
	bool write(const std::string& filename, std::_Ios_Openmode writeType = std::ios::out) const{
#else
	bool write(const std::string& filename, std::ios_base::openmode writeType = std::ios::out) const{
#endif //WIN32
		std::fstream ofs(filename.c_str(), writeType);
		if(!ofs){
			dump();
		}else{
			dump(ofs);
		}
		ofs.close();
		
		return true;
	}
	
	void dump(std::ostream& ofs = std::cout) const{
		int last = this->size() - 1;
		for(int i=0; i<last; i++){
			ofs << (*this)[i] <<std::endl;
		}
		
		if(last > -1 && !(*this)[last].empty()){
			ofs << (*this)[last] <<std::endl;
		}
	}
	
	void setCommentMarks(const String& marks){
		commentMarks = marks.separate(',');
	}
	
	const std::vector<std::string>& getCommentMarks() const{
		return commentMarks;
	}
	
	void clearCommentMarks(){
		commentMarks.clear();
	}
	
	void removeEmptyLine(){
		File file = *this;
		this->clear();
		
		for(size_t i=0; i<file.size(); i++){
			if(!file[i].empty()){
				this->push_back(file[i]);
			}
		}
	}
	
	bool isNull() const{
		return this->empty() ? true : false;
	}

private:
	std::vector<std::string> commentMarks;
};

}//namespace acl

#endif //__ACL__FILE_HPP__
