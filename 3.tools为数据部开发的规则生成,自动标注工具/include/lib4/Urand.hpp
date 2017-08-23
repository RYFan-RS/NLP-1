/*
  ATLAN C++ Library: Urand.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__URAND_HPP__
#define __ACL__URAND_HPP__
#include <vector>
#include <time.h>

namespace acl{

template<int upperBound>
class Urand{
public:
	Urand(bool recycle = false){
		cache.resize(upperBound,0);
		this->recycle = recycle;
		srand(time(0));
	}
	
	int operator ()(){
		if(std::find(cache.begin(),cache.end(),0) == cache.end()){
			if(recycle){
				std::fill(cache.begin(),cache.end(),0);
			}else{
				return -1;
			}
		}
		
		int index;
		
		while(cache[index = rand() % upperBound]);
		cache[index]++;
		
		return index;
	}
	
	
private:
	std::vector<int> cache;
	bool recycle;
};

}//namespace acl

#endif //__ACL__URAND_HPP__
