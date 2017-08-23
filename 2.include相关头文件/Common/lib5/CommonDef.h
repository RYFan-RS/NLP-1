#ifndef __ROBOSAY__BASE__CHAT__COMMONDEF__H__
#define __ROBOSAY__BASE__CHAT__COMMONDEF__H__

#include <string>
#include <vector>
#include <map>
#include <set>

typedef std::set<std::string>				StringSet;
typedef std::map<std::string,std::string> 	StringStringMap;
typedef std::map<int,int> 					IntIntMap;
typedef std::vector<std::string>			StringVector;
typedef std::set<int>						IntSet;
typedef std::map<std::string, IntSet> 		StringIntSetMap;
typedef std::pair<int, int>					IntIntPair;
typedef std::pair<std::string, std::string>	StringStringPair;
typedef std::vector<int>					IntVector;
typedef std::map<std::string, std::vector<std::string> > StringStringVectorMap;
typedef std::map<std::string, int> StringIntMap;

typedef std::pair<float, float> Point;
typedef struct tag_Rectangle{
	Point pointLT;
	Point pointRB;
}Rectangle;

static std::string sEmptyString = "";

 typedef std::map<std::string, StringSet> 		StringStringSetMap;
static bool subset(const StringStringMap& lhs, const StringStringMap& rhs){
	const StringStringMap::const_iterator last = lhs.end();
	StringStringMap::const_iterator itr = lhs.begin();
	for(;last != itr; ++itr){
		const std::map<std::string, std::string>::const_iterator itrFind = rhs.find(itr->first);
		if(itrFind == rhs.end()){
			return false;
		}
	}
	
	return true;
}

static StringStringMap merge(const StringStringMap& lhs, const StringStringMap& rhs){
	StringStringMap oRet = rhs;
	const StringStringMap::const_iterator last = lhs.end();
	StringStringMap::const_iterator itr = lhs.begin();
	for(;last != itr; ++itr){
		oRet[itr->first] = itr->second;
	}
	return oRet;
}

static void set_union(IntSet& lhs, const IntSet& rhs){
	const IntSet::const_iterator last = rhs.end();
	IntSet::const_iterator itr = rhs.begin();
	for(; last != itr; ++itr){
		lhs.insert(*itr);	
	}
}


#endif //__ROBOSAY__BASE__CHAT__COMMONDEF__H__
