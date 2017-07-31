/*
  ATLAN C++ Library: Date.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__DATE_HPP__
#define __ACL__DATE_HPP__
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "String.hpp"

namespace acl{

class Date{
public:
	Date() : m_sDateFormat("%Y-%m-%d") {}
	
	std::string getDate(const char* sDateFormat = "%Y-%m-%d") const {
		char buf[64] = {0};
		struct tm  ts = getCurrentTime();
		
		strftime(buf, sizeof(buf), sDateFormat, &ts);
		
		return normalizeDateString(buf);
	}
	
	static std::string getCurrentDate(const std::string& sSpecialDate=""){
		char buf[64] = {0};
		time_t now;
		struct tm ts;

		// Get current time
		time(&now);
		// Format time.
		ts = *localtime(&now);
		
		// If sepcail date defined, adjust the date
		if(!sSpecialDate.empty()){
			int y,m,d;
			sscanf(sSpecialDate.c_str(),"%d-%d-%d",&y,&m,&d);
			if(y>=2000 && y<=2099){
				ts.tm_year = y-1900;
				ts.tm_mon = m-1;
				ts.tm_mday = d;
				int w = (y-2000)+(y-2000)/4+20/4-2*20+26*(m+1)/10+d-1;
				ts.tm_wday = w % 7;
			}
		}
		
		strftime(buf, sizeof(buf), "%Y-%m-%d", &ts);
		
		return normalizeDateString(buf);
	}
	
	static std::string shiftDate(const std::string& sDate, bool bForward){
		int nYear,nMonth,nDay;
		sscanf(sDate.c_str(),"%d-%d-%d",&nYear,&nMonth,&nDay);
		
		if(bForward == true){ // shift to the day after the give date
			int nMaxDay = getMaxDayOfMonth(nYear, nMonth);
			nDay += 1;
			if(nDay > nMaxDay){
				nDay -= nMaxDay;
				nMonth += 1;
				if(nMonth > 12){
					nYear += 1;
					nMonth = 1;
				}
			}
		}else{// shift to the day before the give date
			nDay -= 1;
			if(nDay < 1){
				nMonth -= 1;
				if(nMonth < 1){
					nYear -= 1;
					nMonth = 12;
					nDay = 31;
				}else{
					nDay = getMaxDayOfMonth(nYear, nMonth);
				}
			}
		}
		
		char buf[64] = {0};
		sprintf(buf,"%04d-%02d-%02d",nYear,nMonth,nDay);
		
		return buf;
	}
	
	static std::string shiftMonth(const std::string& sDate, bool bForward){
		int nYear,nMonth,nDay;
		sscanf(sDate.c_str(),"%d-%d-%d",&nYear,&nMonth,&nDay);
		
		if(bForward == true){ // shift to the month after the give date
			nMonth += 1;
			if(nMonth > 12){
				nYear += 1;
				nMonth = 1;
			}
		}else{// shift to the month before the give date
			nMonth -= 1;
			if(nMonth < 1){
				nYear -= 1;
				nMonth = 12;
				nDay = 31;
			}else{
				nDay = getMaxDayOfMonth(nYear, nMonth);
			}
		}
		
		char buf[64] = {0};
		sprintf(buf,"%04d-%02d-%02d",nYear,nMonth,nDay);
		
		return buf;
	}
	
	static std::string shiftYear(const std::string& sDate, bool bForward){
		int nYear,nMonth,nDay;
		sscanf(sDate.c_str(),"%d-%d-%d",&nYear,&nMonth,&nDay);
		
		if(bForward == true){ // shift to the year after the give date
			nYear += 1;
		}else{// shift to the year before the give date
			nYear -= 1;
		}
		
		char buf[64] = {0};
		sprintf(buf,"%04d-%02d-%02d",nYear,nMonth,nDay);
		
		return buf;
	}
	
	int getWeekDay(int n = 0) const {
        struct tm  ts = getCurrentTime();
		
		if(n < -1 || n > 6)
			n = 0;
		
		return ts.tm_wday+n;
    }
	
	int getYear(int n = 0) const {
        struct tm  ts = getCurrentTime();
		
		return ts.tm_year+n+1900;
    }
	
	std::string getYearMonth(int n = 0) const {
		char buf[64] = {0};
        struct tm  ts = getCurrentTime();
		
		if(n < -1 || n > 12)
			n = 0;
		
		ts.tm_mon += n;
		if(ts.tm_mon > 11){
			ts.tm_mon -= 12;
			ts.tm_year += 1;
		}
		
		strftime(buf, sizeof(buf), "%Y-%m", &ts);
		
		return normalizeDateString(buf);
    }
	
	std::string getRelativeDate(int n) const{
		char buf[64] = {0};
        struct tm  ts = getCurrentTime();
		
		if(n <= 0){
			return getDate();
		}
		
		int nMaxDay = getMaxDayOfMonth(ts.tm_year, ts.tm_mon+1);
		ts.tm_mday += n;
		while(ts.tm_mday > nMaxDay){
			ts.tm_mday -= nMaxDay;
			ts.tm_mon += 1;
			while(ts.tm_mon > 11){
				ts.tm_year += 1;
				ts.tm_mon = 0;
			}
			nMaxDay = getMaxDayOfMonth(ts.tm_year, ts.tm_mon+1);
		}
		
		strftime(buf, sizeof(buf), m_sDateFormat, &ts);
		
		return normalizeDateString(buf);
	}
	
	struct tm getCurrentTime() const {
		time_t now;
		struct tm ts;
		
		// Get current time
		time(&now);
		// Format time.
		ts = *localtime(&now);
		
		// If sepcail date defined, adjust the date
		if(!m_sSpecialDate.empty()){
			int y,m,d;
			sscanf(m_sSpecialDate.c_str(),"%d-%d-%d",&y,&m,&d);
			if(y>=2000 && y<=2099){
				ts.tm_year = y-1900;
				ts.tm_mon = m-1;
				ts.tm_mday = d;
				int w = (y-2000)+(y-2000)/4+20/4-2*20+26*(m+1)/10+d-1;
				ts.tm_wday = w % 7;
			}
		}
		
		return ts;
	}
	
	static int getMaxDayOfMonth(int nYear,int nMonth){
		int nMaxDay;
		
		if ((nYear%400==0) || (nYear%4==0 && nYear%100!=0))
			nMaxDay=29;
		else 
			nMaxDay=28;
			
		switch(nMonth){
			case 2: return nMaxDay;
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12: return 31;
			case 4:;
			case 6:;
			case 9:;
			case 11: return 30;
			default: return 0; 
		}
	}
	
	int getDayOfYear(int y,int m,int d) const{
		int a;
		
		if ((y%400==0)||(y%4==0&&y%100!=0))
			a=29;
		else
			a=28;
		
		switch(m){
			case 1 :	return d;
			case 2 :	return (31+d);
			case 3 :	return (31+a+d);
			case 4 :	return (62+a+d);
			case 5 :	return (92+a+d);
			case 6 :	return (123+a+d);
			case 7 :	return (153+a+d);
			case 8 :	return (184+a+d);
			case 9 :	return (215+a+d);
			case 10:	return (245+a+d);
			case 11:	return (276+a+d);
			case 12:	return (306+a+d);
			default:	return 0;
		}
	}
	
	static std::string normalizeDateString(acl::String sDate){
		std::string sNewDate;
		
		std::vector<std::string> v = sDate.separate("-");
		if(v.size() != 3) return sDate;
		
		// 年
		sNewDate += v[0] + "-";
		// 月
		if(v[1].size() != 2) sNewDate += "0" + v[1] + "-";
		else sNewDate += v[1] + "-";
		// 日
		if(v[2].size() != 2) sNewDate += "0" + v[2];
		else sNewDate += v[2];
		
		return sNewDate;
	}
	
	void setSpecialDate(const std::string& sDate){
		m_sSpecialDate = sDate;
	}
	const std::string& getSpecialDate() const{
		return m_sSpecialDate;
	}
	
	static std::string adjustDateString(const std::string& sReferDate, const std::string& sDate, int nType){
		int nRYear,nRMonth,nRDay;
		int nYear,nMonth,nDay;
		char buf[64] = {0};
		
		sscanf(sReferDate.c_str(),"%d-%d-%d",&nRYear,&nRMonth,&nRDay);
		sscanf(sDate.c_str(),"%d-%d-%d",&nYear,&nMonth,&nDay);
		
		if(nType == 1){// day
			if(nDay > nRDay){
				nMonth = nRMonth;
				nYear = nRYear;
			}else{
				nMonth = nRMonth + 1;
				nYear = nRYear;
				if(nMonth == 13){
					nMonth = 1;
					nYear++;
				}
			}
		}else if(nType == 2){// month day
			if(nMonth > nRMonth){
				nYear = nRYear;
			}else{
				nMonth = nRMonth + 1;
				nYear = nRYear;
				if(nMonth == 13){
					nMonth = 1;
					nYear++;
				}
			}
		}else{// year month day
			if(nYear < nRYear){
				nYear = nRYear + 1;
			}
		}
		
		sprintf(buf,"%4d-%d-%d",nYear,nMonth,nDay);
		
		return normalizeDateString(buf);
	}
private:
	const char* m_sDateFormat;
	std::string m_sSpecialDate;
};

}//namespace acl

#endif //__ACL__DATE_HPP__
