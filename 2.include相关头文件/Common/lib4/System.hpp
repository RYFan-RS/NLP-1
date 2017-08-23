/*
  ATLAN C++ Library: System.hpp
  
  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__SYSTEM_HPP__
#define __ACL__SYSTEM_HPP__
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef WIN32
#include <windows.h>
#include <dos.h>
#include <direct.h>
#include <fcntl.h>
#include <io.h>
#else
#include <dirent.h>
#include <fcntl.h>
#include <sys/param.h>
#include <unistd.h>
#endif
namespace acl{

class System{
public:
	static std::string getCurrentPath(){
		char buf[256];
		if(getcwd(buf,sizeof(buf)) != NULL)
			return buf;
		else return ".";
	}
	
	static bool isFileExist(const std::string& path){
		struct stat buf;
		if (!(stat(path.c_str(), &buf) == 0)) return false;
		return (buf.st_mode & S_IFREG) != 0;
	}
	
	static bool removeLockFile(const std::string& path){
		std::string sName = path + ".lock";
		
		for(int i=0; i<3; i++){
			if(remove(sName.c_str()) == 0){
				return true;
			}else{
				sleep(1);
			}
		}
		
		return false;
	}
	
	static bool createLockFile(const std::string& path){
		std::string sName = path + ".lock";
		FILE * pFile;
		
		for(int i=0; i<3; i++){
			if(isFileExist(sName) == false){
				pFile = fopen (sName.c_str(),"w");
				if (pFile!=NULL){
					fclose (pFile);
					return true;
				}
			}else{
				sleep(1);
			}
		}
		
		return false;
	}
};

}//namespace acl

#endif //__ACL__SYSTEM_HPP__
