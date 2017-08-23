/*
  ATLAN C++ Library: Type.hpp
  
  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__TYPE_HPP__
#define __ACL__TYPE_HPP__
#include <string>
#include <sstream>

namespace acl{

class Type{
public:
	template<class FROM, class TO>
	static TO convert(const FROM& from){
		std::stringstream ss(from);
		TO to;
		ss >> to;
		return to;
	}
	
	template<class FROM, class TO>
	static TO convertString(const FROM& from){
		std::stringstream ss(from);
		TO to = ss.str();
		return to;
	}
};

}//namespace acl

#endif //__ACL__TYPE_HPP__
