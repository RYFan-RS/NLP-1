#include "ChatResult.h"
#include "string.h"
#include "Common/lib4/String.hpp"

#define VRESULT_SEPARATE "!@#$%^&*"

namespace robosay{
namespace base{

ChatResult::ChatResult()
	: std::deque<std::string>(3){

}

ChatResult::~ChatResult(){

}

std::string ChatResult::toStr(){
	std::string sRet;
	const size_t uCnt = this->size();
	for(size_t uIdx = 0u; uCnt > uIdx; ++uIdx){
		sRet += this->front() + VRESULT_SEPARATE;
		this->pop_front();
	}
	return sRet;
}

bool ChatResult::fromStr(const std::string& str){
	this->clear();
	acl::String as(str);
	StringVector vStr = as.separate(VRESULT_SEPARATE);
	const StringVector::const_iterator last = vStr.end();
	StringVector::const_iterator itr = vStr.begin();
	for(; last != itr; ++itr){
        ChatResult::push_back(*itr);
        /*size_t uPos = itr->find("<TableXMLAnswer>");
        if(uPos != std::string::npos){
            std::string sLeft = itr->substr(0, uPos);
            std::string sRight = itr->substr(uPos + strlen("<TableXMLAnswer>"));
            size_t uEnd = sRight.find("</TableXMLAnswer>");
            if(uEnd != std::string::npos){
                sRight = sRight.substr(uEnd + strlen("</TableXMLAnswer>"));
            }
		    ChatResult::push_back(sLeft + sRight);
        }else{
		    ChatResult::push_back(*itr);
        }*/
	}
	return true;
}

bool ChatResult::push_front(const std::string& str){
	if(str.empty()){
		return false;
	}
	this->std::deque<std::string>::push_front(str);
	return true;
}

bool ChatResult::push_back(const std::string& str){
	if(str.empty()){
		return false;
	}
	this->std::deque<std::string>::push_back(str);
	return true;
}

}//namespace base
}//namespace robosay
