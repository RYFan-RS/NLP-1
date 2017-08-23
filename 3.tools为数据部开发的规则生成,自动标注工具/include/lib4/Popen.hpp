/*
  ATLAN C++ Library: Popen.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__POPEN_HPP__
#define __ACL__POPEN_HPP__
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <stdlib.h>
#include "Line.hpp"
#include "Definations.hpp"

namespace acl{

class Popen{
public:
	static bool execute(const std::string& sCmd, acl::Line& line) {
		FILE *fp;
		char buf[1024];
		
		if((fp = popen( sCmd.c_str(), "r" )) == NULL){
			PRINT_ERROR("Failed to open pipe!")
			return false;
		}
		
		while(fgets (buf , sizeof(buf) , fp) != NULL){
			line += buf;
		}
		
		pclose( fp ); 
		
		if(line.find("ERROR:") != std::string::npos){
			return false;
		}
		
		return true;
	}
};

}//namespace acl

#endif //__ACL__POPEN_HPP__
