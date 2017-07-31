/*
  ATLAN C++ Library: Debug.hpp
  
  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__DEBUG_HPP__
#define __ACL__DEBUG_HPP__
#ifdef WIN32
#pragma warning(disable: 4786)
#include <Windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif //WIN32
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <set>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>


namespace acl{

namespace debug{

const int LEVEL0 = 0;
const int LEVEL1 = 1;
const int LEVEL2 = 2;
const int LEVEL3 = 3;
const int LEVEL4 = 4;
const int LEVEL5 = 5;
const int LEVEL6 = 6;
const int LEVEL7 = 7;
const int LEVEL8 = 8;
const int LEVEL9 = 9;

#define MESSAGE(ofs, msg_stream, LEVEL) \
	log(ofs, "MESSAGE:", msg_stream.str(), __FILE__, __LINE__, LEVEL); \
        msg_stream.str("") \
	
#define WARNNING(ofs, msg_stream, LEVEL) \
	log(ofs, "WARNMSG:", msg_stream.str(), __FILE__, __LINE__, LEVEL); \
        msg_stream.str("") \
	
#define ERRMSG(ofs, msg_stream, LEVEL) \
	log(ofs, "ERR_MSG:", msg_stream.str(), __FILE__, __LINE__, LEVEL); \
        msg_stream.str("") \
        
#define MAX_FILE_SIZE 1024*1024*200

#define TAB_CHAR "\t"

#define MAX_LOG_COUNT 10

static std::map<std::string,std::string> mFileNames;
static int nRecordLevel = 9;

// format : YY/MM/DD 00:00:00
static std::string get_time_string()
{
	char buf[32];
#ifdef WIN32
	SYSTEMTIME sys;
	GetLocalTime( &sys );
	sprintf_s( buf,sizeof(buf),"%4d/%02d/%02d %02d:%02d:%02d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond); 
#else
	time_t now;
	struct tm  *timenow;
	time(&now); 
	timenow = localtime(&now);
	snprintf(buf,32, "%4d/%02d/%02d %02d:%02d:%02d",
			1900+timenow->tm_year,timenow->tm_mon+1,timenow->tm_mday,timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
#endif
	return buf;
}

static std::string sPreFileName;

static std::string generate_new_file_name(const std::string &fileName){
	std::string baseName,extendName,newName,cache;
	int count,pos,i;
	char number[32];
	
	pos = fileName.rfind(".");
	if(pos == (int)std::string::npos){
		pos = fileName.length();
	}
	baseName = fileName.substr(0,pos);
	extendName = fileName.substr(pos);
	for(i=(int)baseName.length()-1;i>=0;i--){
		if(baseName[i]<'0' || baseName[i]>'9'){
			pos = i;
			break;
		}
	}
	if(pos == (int)baseName.length()-1){
		cache = "0";
	}else{
		cache = baseName.substr(pos+1);
	}
	baseName = baseName.substr(0,pos+1);
	count = atoi(cache.c_str());
	count++;
#ifdef WIN32
	sprintf_s(number,"%d",count);
#else
	sprintf(number,"%d",count);
#endif
	newName = baseName;
	newName += number;
	newName += extendName;
	
	std::string delName = baseName;
	count -= MAX_LOG_COUNT;
#ifdef WIN32
	sprintf_s(number,"%d",count);
#else
	sprintf(number,"%d",count);
#endif	
	delName += number;
	delName += extendName;
	if(!access(delName.c_str(), 0)){
		remove(delName.c_str());
	}
	
	return newName;
}

static std::string toStr(const int& n){
	std::string s;
	char cache[32];
	
#ifdef WIN32
	sprintf_s(cache,"%d",n);
#else
	sprintf(cache,"%d",n);
#endif
	
	s = cache;
	
	return s;
}

template<class T>
static std::string toStr(const std::set<T>& setT){
	typename std::set<T>::const_iterator iter;
	std::stringstream ss;
	
	ss<<"{";
	for(iter=setT.begin(); iter!=setT.end(); ++iter){
		if(iter != setT.begin()) ss<<",";
		ss<<(*iter);
	}
	ss<<"}";
	
	return ss.str();
}

template<class T>
static std::string toStr(const std::vector<T>& vT){
	std::stringstream ss;
	
	ss<<"{";
	for(size_t i=0; i<vT.size(); i++){
		if(i != 0) ss<<",";
		ss<<vT.at(i);
	}
	ss<<"}";
	
	return ss.str();
}

static std::string toStr(const std::map<std::string,std::string>& m){
	std::stringstream ss;
	std::map<std::string,std::string>::const_iterator iter;
	
	ss<<"{";
	for(iter=m.begin(); iter!=m.end(); ++iter){
		if(iter != m.begin()) ss<<",";
		ss<<iter->first<<"-"<<iter->second;
	}
	ss<<"}";
	
	return ss.str();
}

template<class T>
static std::string toStrPlus(const std::set<T>& setT){
	typename std::set<T>::const_iterator iter;
	std::stringstream ss;
	
	ss<<"{";
	for(iter=setT.begin(); iter!=setT.end(); ++iter){
		if(iter != setT.begin()) ss<<",";
		ss<<(*iter).toStr();
	}
	ss<<"}";
	
	return ss.str();
}

template<class T>
static std::string toStrPlus(const std::vector<T>& vT){
	std::stringstream ss;
	
	ss<<"{";
	for(size_t i=0; i<vT.size(); i++){
		if(i != 0) ss<<",";
		ss<<vT.at(i).toStr();
	}
	ss<<"}";
	
	return ss.str();
}
static void log(std::ostream &ofs, const std::string & head, const std::string &msg,
			const std::string &file, const int &line, const int& level = 0){
	std::string s,sLevel;
	
	// read if the level should be recorded
	if(level > nRecordLevel) return;
	
	// make the level prefix mark
	sLevel = "";
	for(int i=0; i<level; i++){
		sLevel += "| ";
	}
	
	// write the message to stream
	s = "[" + get_time_string() + "]" + head + TAB_CHAR + sLevel + msg + TAB_CHAR;
	s += file + ":"+ toStr(line) + " ";
	ofs << s << std::endl << std::flush;
}
			
static int get_file_size(const std::string &fileName){
	int size;
	struct stat file_stat;
	if(!stat(fileName.c_str(), &file_stat)){
		size = file_stat.st_size;
		return size;
	}else{
		return -1;
	}
}

static void log(std::string &fileName, const std::string & head, const std::string &msg, 
			const std::string &file, const int &line, const int& level = 0){
	while(get_file_size(fileName.c_str()) >= MAX_FILE_SIZE){
		if(sPreFileName.empty()){
			sPreFileName = fileName;
		}
        
		std::string newfileName = generate_new_file_name(sPreFileName);
		if(!access(newfileName.c_str(), 0)){
			remove(newfileName.c_str());
		}
		rename(fileName.c_str(), newfileName.c_str());
		sPreFileName = newfileName;
	}
    
	std::fstream ofs(fileName.c_str(),std::ios::out|std::ios::app);
	log(ofs,head,msg,file,line,level);
	ofs.close();
}

static void log(const std::string &fileName, const std::string & head, const std::string &msg,
			const std::string &file, const int &line, const int& level = 0){
	std::map<std::string,std::string>::iterator ite;
	ite = mFileNames.find(fileName);
	if(ite == mFileNames.end()){
		mFileNames.insert(std::pair<std::string,std::string>(fileName,fileName));
		ite = mFileNames.find(fileName);
	}
	std::string tmpName = ite->second;
	log(tmpName,head,msg,file,line,level);
	ite->second = tmpName;
}

}//namespace debug

}//namespace acl

#endif //__ACL__DEBUG_HPP__

