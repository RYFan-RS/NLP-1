/*
  ATLAN C++ Library: DateString.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__DATE_STRING_HPP__
#define __ACL__DATE_STRING_HPP__
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Date.hpp"
#include "String.hpp"

namespace acl{

class DateString{
public:
	DateString(){
		reset();
	}
	
	bool read(const acl::String& sDate){
		sscanf(sDate.c_str(),"%d-%d-%d",&m_nYear,&m_nMonth,&m_nDay);
		
		return true;
	}
	
	bool readRange(const acl::String& sDate, const char* sSeparator, const std::string& sSpecialDate=""){
		std::vector<std::string> v = sDate.separate(sSeparator);
		
		sscanf(v[0].c_str(),"%d-%d-%d",&m_nYear,&m_nMonth,&m_nDay);
		
		sscanf(v[1].c_str(),"%d-%d-%d",&m_nRangeYear,&m_nRangeMonth,&m_nRangeDay);
		
		m_bIsRange = true;
		
		std::string sCurDate = acl::Date::getCurrentDate(sSpecialDate);
		
		if(v[0] > v[1]){
			int nCurY,nCurM,nCurD;
			sscanf(sCurDate.c_str(),"%d-%d-%d",&nCurY,&nCurM,&nCurD);
			m_nYear = nCurY;
			m_nMonth = nCurM;
			m_nDay = nCurD;
		}
		
		return true;
	}
	
	std::string toStr(const char* sSeparate) const{
		std::stringstream ss;
		
		if(m_bIsRange){
			ss<<m_nYear<<"-"<<m_nMonth<<"-"<<m_nDay;
			std::string sBegDate = acl::Date::normalizeDateString(ss.str());
			ss.str("");
			ss<<m_nRangeYear<<"-"<<m_nRangeMonth<<"-"<<m_nRangeDay;
			std::string sEndDate = acl::Date::normalizeDateString(ss.str());
			return sBegDate + sSeparate + sEndDate;
		}else{
			ss<<m_nYear<<"-"<<m_nMonth<<"-"<<m_nDay;
			return acl::Date::normalizeDateString(ss.str());
		}
	}
	
	void reset(){
		m_bIsRange = false;
		m_nYear = -1;
		m_nMonth = -1;
		m_nDay = -1;
		m_nRangeYear = -1;
		m_nRangeMonth = -1;
		m_nRangeDay = -1;
	}
private:
	bool m_bIsRange;
	int m_nYear;
	int m_nMonth;
	int m_nDay;
	int m_nRangeYear;
	int m_nRangeMonth;
	int m_nRangeDay;
};

}//namespace acl

#endif //__ACL__DATE_STRING_HPP__
