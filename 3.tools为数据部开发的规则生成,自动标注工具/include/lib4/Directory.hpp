/*
  ATLAN C++ Library: Directory.hpp
  
  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__DIRECTORY_HPP__
#define __ACL__DIRECTORY_HPP__

#ifndef WIN32
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/param.h>
#include <unistd.h>
#include <vector>
#include <set>
#include "String.hpp"
#include "FileName.hpp"

namespace acl{

class Directory : public std::vector<FileName>{
public:
	Directory(){}
	Directory(const std::string& path){
		this->m_path = path;
	}
	
	void setPath(const std::string& path){
		this->m_path = path;
	}
	
	const std::string& getPath() const{
		return m_path;
	}
	
	void setFilter(const acl::String& sFilter){
		std::vector<std::string> filter = sFilter.separate(",");
		std::copy(filter.begin(),filter.end(),std::inserter(m_filter,m_filter.begin()));
	}
	
	std::string getFilter() const{
		std::string sFilter;
		
		for(std::set<std::string>::const_iterator i=m_filter.begin(); i!=m_filter.end(); ++i){
			if(i != m_filter.begin()) sFilter += " ";
			sFilter += *i;
		}
		
		return sFilter;
	}
	
	void dump(std::ostream& ofs = std::cout) const{
		ofs<<"Path  : "<<m_path<<std::endl;
		ofs<<"Filter: "<<getFilter()<<std::endl;
		ofs<<size()<<" file(s):"<<std::endl;
		for(size_t i=0; i<size(); i++){
			ofs<<"\t"<<at(i)<<std::endl;
		}
	}

/*
	// this code is for Windows
	std::string wcharToChar(wchar_t *orig){
		size_t origsize = wcslen(orig) + 1;
		const size_t newsize = 100;
		size_t convertedChars = 0;
		char nstring[newsize];
		wcstombs_s(&convertedChars, nstring, origsize, orig, _TRUNCATE);

		return nstring;
	}
*/
	
	bool read(){
		DIR *dp;
		struct dirent *dirp;
		
		if((dp  = opendir(m_path.c_str())) == NULL) {
			std::cerr << "Error in opening " << m_path << std::endl;
			return false;
		}
		
		while ((dirp = readdir(dp)) != NULL) {
			if(dirp->d_name[0] == '.') continue;
			if(!m_filter.empty()){
				acl::FileName fileName = dirp->d_name;
				if(m_filter.find(fileName.getExtension()) == m_filter.end()) continue;
			}
			this->push_back(dirp->d_name);
		}
		
		closedir(dp);
		
		return true;
	}
	
	bool recursivelyRead(const std::string& externPath = ""){
		DIR *dp;
		struct dirent *dirp;
		std::vector<std::string> childPaths;
		acl::FileName fileName;
		std::string path = m_path + externPath;
		
		if((dp  = opendir(path.c_str())) == NULL) {
			std::cerr << "Error in opening " << path << std::endl;
			return false;
		}
		
		while ((dirp = readdir(dp)) != NULL) {
			if(dirp->d_name[0] == '.') continue;
			fileName = dirp->d_name;
			
			if(isDirectory(path + "/" + fileName)){
				childPaths.push_back(externPath + "/" + fileName);
				continue;
			}
			if(!m_filter.empty()){
				if(m_filter.find(fileName.getExtension()) == m_filter.end()) continue;
			}
			this->push_back(externPath + "/" + fileName);
		}
		
		closedir(dp);
		
		for(size_t i=0; i<childPaths.size(); i++){
			recursivelyRead(childPaths[i]);
		}
		
		return true;
	}
	
	bool copyTo(const std::string& path){
		std::string command;
		for(size_t i=0; i<this->size(); i++){
			command = "mkdir -p " + path + this->at(i).getPath();
			system(command.c_str());
			command = "cp -uf " + m_path + this->at(i) + " " + path + this->at(i);
			std::cout<<command<<std::endl;
			system(command.c_str());
		}
	}
private:
	bool isFile(const std::string& fileName) const{
		struct stat buf;
		if (!(stat(fileName.c_str(), &buf) == 0)) {return false;}
		return (buf.st_mode & S_IFREG) != 0;
	}
	
	bool isDirectory(const std::string& fileName) const{
		struct stat buf;
		if (!(stat(fileName.c_str(), &buf) == 0)) {return false;}
		return (buf.st_mode & S_IFDIR) != 0;
	}
private:
	std::string m_path;
	std::set<std::string> m_filter;
};

}//namespace acl

#endif //NDEF WIN32

#endif //__ACL__DIRECTORY_HPP__
