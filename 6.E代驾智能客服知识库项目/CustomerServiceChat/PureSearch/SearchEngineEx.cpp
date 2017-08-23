#include "CustomerServiceChat/PureSearch/SearchEngineEx.h"

#ifndef PRINT_LOG
#define PRINT_LOG
#endif
#include "Common/lib5/WHL_LogInstance.hpp"

whl::mutex_lock whl::logger::m_lock;
whl::logger* whl::logger::m_instance = NULL;

namespace aisms{        
	aisms::GOption gOption;}

namespace robosay{
namespace base{

SearchEngineEx::SearchEngineEx(){

}

SearchEngineEx::~SearchEngineEx(){

}

bool SearchEngineEx::init(const std::string& sConfigFile) {
	acl::String sRootFile = "";
	std::cerr<<"Use config file: "<<sConfigFile<<std::endl;
	if(m_vKB.init(sConfigFile) == false){
		LOG_FAILE_CALL_PARAM("SearchEngineEx::init", "m_vKB.init", sConfigFile.c_str());
		return false;
	}
	std::cerr<<"Initialization finished."<<std::endl;
	return true;
}

// 调用接口
bool SearchEngineEx::process(const std::string& sInput, std::vector<std::pair<std::string,std::string> >& resultList,
								std::string& errorInfo){
	acl::String sRet;
	std::string sChatState;
	Result_info resultInfo;
	std::vector<std::pair<std::string,std::string> > result;

	resultList.clear();
	// 检查输入文是否为标准的UTF8字符
	if(checkInputLegality(sInput,sRet) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("SearchEngineEx::process", "SearchEngineEx::checkInputLegality", sInput.c_str()));
		errorInfo = sRet;
		return false;
	}


	bool bRet = m_vKB.execute(sInput, true, sChatState, resultInfo, resultList);
	if(!bRet){
        DLOGGER->print(true, FAILE_CALL_PARAM("SearchEngineEx::process","m_vKB.execute", sInput.c_str()));
		errorInfo = "查找不到答案";
	}

	return bRet;
}

bool SearchEngineEx::checkInputLegality(const acl::String& sIn, acl::String& sRet) const{
	if(sIn.empty()){
		sRet = "您好，您的输入文为空。";
		ELOGGER->print(true, FAILE_CALL_PARAM("SearchEngineEx::checkInputLegality","sIn.empty", sRet.c_str()));
		return false;
	}

	StringVector vChar = sIn.toCharactor();
	if(vChar.empty()){
		sRet = "你好，您的输入文有未识别字符。";
		ELOGGER->print(true, FAILE_CALL_PARAM("SearchEngineEx::checkInputLegality","vChar.empty",sIn.c_str()));
		return false;
	}
	return true;
}

}//namespace base
}//namespace robosay
