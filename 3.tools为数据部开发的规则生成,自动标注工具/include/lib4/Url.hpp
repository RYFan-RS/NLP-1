/*
  ATLAN C++ Library: Url.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.

  Author: atlantise522@gmail.com
*/
#ifndef __ACL__URL_HPP__
#define __ACL__URL_HPP__
#include <iostream>

namespace acl{

static const char safe[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

class Url{
public:
	static std::string encode(const std::string &uri){
		std::string ret;
		const unsigned char *ptr = (const unsigned char *)uri.c_str();
		ret.reserve(uri.length());

		for (; *ptr ; ++ptr)
		{
			if (!safe[*ptr])
			{
				char buf[5];
				memset(buf, 0, 5);
#ifdef WIN32
				_snprintf_s(buf, 5, "%%%X", (*ptr));
#else
				snprintf(buf, 5, "%%%X", (*ptr));
#endif
				ret.append(buf);	
			}
			else if (*ptr==' ')
			{
				ret+='+';
			}
			else{
				ret += *ptr;
			}
		}
		return ret;
	}

	static std::string decode(const std::string &uri){
		//Note from RFC1630:  "Sequences which start with a percent sign
		//but are not followed by two hexadecimal characters (0-9,A-F) are reserved
		//for future extension"
		const unsigned char *ptr = (const unsigned char *)uri.c_str();
		std::string ret;
		ret.reserve(uri.length());
		for (; *ptr; ++ptr)
		{
			if (*ptr == '%')
			{
				if (*(ptr + 1))
				{
					char a = *(ptr + 1);
					char b = *(ptr + 2);
					if (!((a >= 0x30 && a < 0x40) || (a >= 0x41 && a < 0x47))) continue;
					if (!((b >= 0x30 && b < 0x40) || (b >= 0x41 && b < 0x47))) continue;
					char buf[3];
					buf[0] = a;
					buf[1] = b;
					buf[2] = 0;
					ret += (char)strtoul(buf, NULL, 16);
					ptr += 2;
					continue;
				}
			}
			if (*ptr=='+')
			{
				ret+=' ';
				continue;
			}
			ret += *ptr;
		}
		return ret;
	}
};

}//namespace acl

#endif //__ACL__URL_HPP__
