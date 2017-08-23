#include "WHL_Define.hpp"
#include "WHL_Log.hpp"
#include "WHL_LogInstance.hpp"
whl::mutex_lock whl::logger::m_lock;
whl::logger* whl::logger::m_instance = NULL;
#define FAILE_CP(x,y,z) 		"FILE: %s LINE: %d\nERROR: in [%s], failed to call [%s], parameter is [%s]\n", __FILE__, __LINE__, x, y, z
int main(int argc, char* argv[]){
	whl::daily_log 	m_errLog;
	whl::try_log 	m_dataLog;
	int nRet = m_errLog.init("./log", "error", true);
	if(nRet){
		LOG_FAILE_CALL_PARAM("logger::logger", "m_errLog.init", "./log");
		exit(0);
	}
	
	nRet = m_dataLog.init("./try_log", "data", true);
	if(nRet){
		LOG_FAILE_CALL_PARAM("logger::logger", "m_dataLog.init", "./try_log");
		exit(0);
	}
	DLOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "config.read", "aaa"));
	ELOGGER->print(true, "VKnowledgeBase::initaaa");
	ELOGGER->print(true, "VKnowledgeBase::initaaa%s%d%s","cc",__LINE__,"aa");
	ELOGGER->print(true, "VKnowledgeBase::initaaa%s%s",__FILE__,"c");
	whl::logger::get_instance()->get_error_log()->print(true, "failed to call [%s], parameter is [%s]\n", "config.read", "aaa");
	whl::logger::get_instance()->get_error_log()->print(true, "FILE: %s LINE: %d\n", __FILE__, __LINE__);
	DLOGGER->print(true, "FILE: %s LINE: %d\nERROR: in [%s]", __FILE__, __LINE__,"aaa");
	m_dataLog.print(true, "FILE: %s LINE: %d\nERROR: in [%s], failed to call [%s], parameter is [%s]\n", __FILE__, __LINE__,"VKnowledgeBase::init", "config.read", "aaa");
	m_errLog.print(true, FAILE_CP("VKnowledgeBase::init", "config.read", "aaa"));
    return 0;
}
