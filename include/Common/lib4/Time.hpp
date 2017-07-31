/*
  ATLAN C++ Library: Time.hpp
  
  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__TIME_HPP__
#define __ACL__TIME_HPP__

#ifndef WIN32

#include <iostream>
#include <map>
#include <sys/time.h>

namespace acl{

class Time{
public:
	static std::string getSysDate(const char* fmt = "%Y%m%d_%H:%M:%S"){
		struct timeval tv;
		gettimeofday(&tv , NULL);
		struct tm* local_time = localtime(&(tv.tv_sec));
		char formed_date[32];
		strftime(formed_date,32,fmt,local_time);
		return formed_date;
	}
	
	static std::string getCurrentDate(){
		return getSysDate("%Y%m%d");
	}
	
	static std::string getCurrentTime(){
		return getSysDate("%H:%M:%S");
	}
	// nInterval = nBase(e.g. 5) + nRange(e.g. 0-5) = random in {5,10}
	static int generateInterval(int nBase, int nRange){
		srand( (unsigned)time( NULL ) );
		
		return rand()% nRange + nBase;
	}
	
	bool start(const std::string& name){
		std::vector<long> timenum;
		struct timeval t;
		
		gettimeofday(&t , NULL);
		timenum.push_back(t.tv_sec);
		timenum.push_back(t.tv_usec);
		timeList.insert(std::pair<std::string,std::vector<long> >(name,timenum));
		
		return true;
	}
	
	bool end(const std::string& name){
		std::map<std::string,std::vector<long> >::iterator i;
		
		i = timeList.find(name);
		if(i == timeList.end()){
			std::cerr<<"ERR)R"<<std::endl;
			return false;
		}else{
			struct timeval t;
			
			gettimeofday(&t , NULL);
			(i->second).push_back(t.tv_sec);
			(i->second).push_back(t.tv_usec);
		}
		
		return true;
	}
	
	void print(const std::string& name, std::ostream &ofs = std::cout){
		std::string sRet = toStr(name);
		
		if(sRet.empty()){
			ofs<<"acl::Time ERROR!!!"<<std::endl;
		}else{
			ofs<<toStr(name)<<std::endl;
		}
	}
	
	std::string toStr(const std::string& name){
		std::map<std::string,std::vector<long> >::iterator i;
		std::stringstream ss;
		
		i = timeList.find(name);
		if(i != timeList.end())
		{
			if( (i->second).size() == 2 ){
				struct timeval t;
				
				gettimeofday(&t , NULL);
				ss<<"TimeCost["<<name<<"] : "<<(t.tv_sec-(i->second)[0])*1000+(t.tv_usec-(i->second)[1])/1000<<" millisecond (";
				ss<<(t.tv_sec-(i->second)[0])*1000000+(t.tv_usec-(i->second)[1])<<" microsecond)";
				timeList.erase(i);
			}else{
				ss<<"TimeCost["<<name<<"] : "<<((i->second)[2]-(i->second)[0])*1000+((i->second)[3]-(i->second)[1])/1000<<" millisecond (";
				ss<<((i->second)[2]-(i->second)[0])*1000000+((i->second)[3]-(i->second)[1])<<" microsecond)";
				timeList.erase(i);
			}
		}
		
		return ss.str();
	}
	
private:
	std::map<std::string,std::vector<long> > timeList;
};

}//namespace acl

#endif //NDEF WIN32

#endif //__ACL__TIME_HPP__
