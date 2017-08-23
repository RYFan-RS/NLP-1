/*
  ATLAN C++ Library: Definations.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__DEFINATIONS_HPP__
#define __ACL__DEFINATIONS_HPP__
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <string>
namespace acl{
#define PRINT(X) std::cout<<X<<"\t@@"__FILE__<<":"<<__LINE__<<std::endl;
#define P(X) PRINT(X)

#define THRIFT_START_SUCCESS_FLAG " THRIFT_SUCCESS"
#define THRIFT_SERVER1_START_SUCCESS_FLAG "Server1 THRIFT_SUCCESS"
#define THRIFT_SERVER2_START_SUCCESS_FLAG "Server2 THRIFT_SUCCESS"
#define THRIFT_ROBOTMANAGER_START_SUCCESS_FLAG "RobotManager THRIFT_SUCCESS"

#define PRINT_ERROR(X) std::cerr<<X<<"\t@@"__FILE__<<":"<<__LINE__<<std::endl;
#define PE(X) PRINT_ERROR(X)

#define PRINT_BOOL(X) std::cout<<((X) == true ? "TRUE" : "FLASE")<<"\t@@"__FILE__<<":"<<__LINE__<<std::endl;
#define PB(X) PRINT_BOOL(X)

#define PRINT_VECTOR(X) std::cout<<__FILE__<<":"<<__LINE__<<"=>size: "<<X.size()<<std::endl;\
		for(size_t _i=0; _i<X.size(); _i++){\
			std::cout<<_i<<": "<<X[_i]<<std::endl;\
		}
#define PV(X) PRINT_VECTOR(X)

#define PRINT_SET(TYPE,X) std::cout<<__FILE__<<":"<<__LINE__<<"=>size: "<<X.size()<<std::endl;\
		int index = 0;\
		for(TYPE::const_iterator _i=X.begin(); _i!=X.end(); ++_i){\
			std::cout<<index++<<": "<<*_i<<std::endl;\
		}
#define PS(TYPE,X) PRINT_SET(TYPE,X)

#define PRINT_STRMAP(X) std::cout<<__FILE__<<":"<<__LINE__<<"=>size: "<<X.size()<<std::endl;\
		std::cout<<"{";\
		for(std::map<std::string,std::string>::const_iterator _i=X.begin(); _i!=X.end(); ++_i){\
			std::cout<<"["<<_i->first<<"-"<<_i->second<<"]";\
		}\
		std::cout<<"}"<<std::endl;
#define PSM(X) PRINT_STRMAP(X)

const std::string EMPTY_STRING;
const std::vector<std::string> EMPTY_VECTOR_STRING;

static int genPIDFile(int port, const char* name="server"){
	int nPid = getpid();
	std::stringstream ss;
	ss<<std::string(name)<<port<<".pid";
	std::ofstream out(ss.str().c_str(), std::ios::out);
	out<<nPid<<std::endl;
	out.close();

	return nPid;
}

}//namespace acl

#endif //__ACL__DEFINATIONS_HPP__
