#ifndef __CYW_LOG_INSTANCE_H_
#define __CYW_LOG_INSTANCE_H_

#include "WHL_Log.hpp"
#include <stdlib.h>

#define LOG_DIR "./logKey"
#define TRY_DIR "./try_logKey" 

#define ELOGGER whl::logger::get_instance()->get_error_log()
#define DLOGGER whl::logger::get_instance()->get_data_log()

namespace whl{
class logger{
public:
	static whl::logger* get_instance(){
		if(!m_instance){
			m_lock.lock();
			try{
				if(!m_instance){
					m_instance = new whl::logger();
				}
				m_lock.unlock();
			}catch(...){
				m_lock.unlock();
				LOG_FAILE_NEW("m_instance");
				exit(0);
			}
		}
		return m_instance;
	}
	
	whl::daily_log* get_error_log(){
		return &m_errLog;
	}
	
	whl::try_log* get_data_log(){
		return &m_dataLog;
	}

private:
	logger()
		: m_errLog()
		, m_dataLog(){
			if (0 != access(LOG_DIR, 0)){
				mkdir(LOG_DIR, S_IRWXO|S_IRWXU);
			}
			int nRet = m_errLog.init(LOG_DIR, "error", true);
			if(nRet){
				LOG_FAILE_CALL_PARAM("logger::logger", "m_errLog.init", "./logKey");
				exit(0);
			}
			
			nRet = m_dataLog.init(TRY_DIR, "data", true);
			if(nRet){
				LOG_FAILE_CALL_PARAM("logger::logger", "m_dataLog.init", "./try_logKey");
				exit(0);
			}
		}
	
	~logger(){}
	
	logger(const logger& rhs);
	
	logger& operator=(const logger& rhs);
	
private:	
	whl::daily_log 	m_errLog;
	whl::try_log 	m_dataLog;
	
	static whl::mutex_lock m_lock;
	static whl::logger* m_instance;
};
}

#endif
