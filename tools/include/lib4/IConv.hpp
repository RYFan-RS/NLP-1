/*
  ATLAN C++ Library: Vector.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__ICONV_HPP__
#define __ACL__ICONV_HPP__
#include <iostream>
#include <vector>
#include <iconv.h>

namespace acl{

class IConv{
public:
	IConv(std::string sFromCharset, std::string sToCharSet) {
		sToCharSet += "//IGNORE";
		sFromCharset += "//IGNORE";
		m_iconv = iconv_open(sToCharSet.c_str(),sFromCharset.c_str());
	}
	
	~IConv() {
		iconv_close(m_iconv);
	}
	
	std::string convert(const std::string& sIn) const{
		std::string sRet;
		
		size_t nIn = sIn.length();
		char* inbuf = new char[nIn];
		memset(inbuf,0,nIn);
		memcpy(inbuf,sIn.c_str(),nIn);
		
		size_t nOut = sIn.length()*3;
		char* outbuf = new char[nOut];
		memset(outbuf,0,nOut);
		
		char* in = inbuf;
		char* out = outbuf;
		iconv(m_iconv,&in,&nIn,&out,&nOut);
		
		sRet = out;
		
		//delete inbuf;
		//delete outbuf;
		
		return sRet;
	}
	
private:
	iconv_t	m_iconv;
};

}//namespace acl

#endif //__ACL__ICONV_HPP__