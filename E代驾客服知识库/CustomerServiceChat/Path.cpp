#include "CustomerServiceChat/Path.h"
#include <stdlib.h>
#include <sstream>
#include "Common/lib5/WHL_LogInstance.hpp"

namespace robosay{
namespace base{

Path::Path(){
	reset();
}

Path::~Path(){

}

void Path::reset(){
	m_vIndexs.clear();
}

bool Path::fromStr(const std::string& sBuf){
	reset();
	if(sBuf == "NULL"){
		ELOGGER->print(true, "Path::fromStr  [sBuf == NULL]\n");
		return true;
	}
	std::string sPos = sBuf, sTmp;
	for(;;){
		int nIndex = sPos.find_first_of('-');
		sTmp.clear();
		if(nIndex >= 0){
			sTmp = sPos.substr(nIndex + 1);
		}
		int n = atoi(sPos.c_str());
		m_vIndexs.push_back(n);
		
		if(!sTmp.empty()){
			sPos = sTmp;
		}else{
			DLOGGER->print(true, "Path::fromStr  [sTmp.empty()]\n");
			break;
		}
	}
	return true;
}

Path::Path(const Path& rhs){
	m_vIndexs = rhs.m_vIndexs;
}

bool Path::isValid() const{
	if(m_vIndexs.empty()) return false;
	return true;
}

std::string Path::toStr() const {
	std::stringstream ss;
	const std::vector<int>::const_iterator last = m_vIndexs.end();
	std::vector<int>::const_iterator itr = m_vIndexs.begin();
	for(; last != itr; ++itr){
		ss<<*itr<<"-";
	}
	std::string str = ss.str();
	if(str.empty()){
		ELOGGER->print(true, "Path::toStr [str.empty()] \n");
		return str;
	}else{
		str.resize(str.length() - 1);
	}
	return str;
}

bool Path::setPath(const void* pVal){
	return true;
}

int Path::getKBIdx() const{
	if(m_vIndexs.empty()){
	ELOGGER->print(true, "Path::getKBIdx [m_vIndexs.empty()] \n");
		return -1;
	}
	return m_vIndexs[0];
}

int Path::GetQAInx()const
{
	if (m_vIndexs.size() < 2)
	{
		ELOGGER->print(true, "Path::GetQAInx [m_vIndexs.size() < 2] \n");
		return -1;
	}

	return m_vIndexs[1];
}


}//namespace base
}//namespace robosay
