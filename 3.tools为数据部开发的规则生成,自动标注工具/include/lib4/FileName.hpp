/*
  ATLAN C++ Library: FileName.hpp
  
  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__FILE_NAME_HPP__
#define __ACL__FILE_NAME_HPP__
#include <iostream>
#include <string>

namespace acl{

class FileName : public std::string{
public:
	FileName() {}
	FileName(const char *buf) : std::string(buf) {}
	FileName(char *buf) : std::string(buf) {}
	FileName(const std::string& s) : std::string(s) {}
	
	// get the file type extension
	std::string getExtension() const{
		const size_t pos = this->rfind('.');
		return (pos == std::string::npos) ? "" : this->substr(pos+1);
	}
	
	// get the name without extension but with the path
	std::string getBaseName() const{
		const size_t pos = this->rfind('.');
		return (pos == std::string::npos) ? static_cast<std::string>(*this) : this->substr(0,pos);
	}
	
	// add a new string between the basename and the extension. e.g.: a.txt -> a.mid.txt
	std::string addMiddleName(const std::string& middleName) const{
		std::string extension = getExtension();
		if(extension.empty()){
			return getBaseName() + "_" + middleName;
		}else{
			return getBaseName() + "." + middleName + "." + extension;
		}
	}
	
	// change the extension
	std::string changeExtension(const std::string& extension) const{
		return getBaseName() + "." + extension;
	}
	
	// get the path of the file
	std::string getPath() const{
		const size_t pos = this->rfind('/');
		return (pos == std::string::npos) ? "./" : this->substr(0,pos);
	}
	
	// get the filename without the path
	std::string getNameWithoutPath() const{
		const size_t pos = this->rfind('/');
		return (pos == std::string::npos) ? this->substr(0) : this->substr(pos+1);
	}
	
	std::string getNameWithoutPathExtension() const{
		std::string sName = getNameWithoutPath();
		const size_t pos = sName.rfind('.');
		return (pos == std::string::npos) ? sName : sName.substr(0, pos);
	}
};

}//namespace acl

#endif //__ACL__FILE_NAME_HPP__
