#include "CustomerServiceChat/Path.h"
#include "CustomerServiceChat/GeneralChat/Path.h"
#include "CustomerServiceChat/QA.h"

namespace robosay{
namespace general{

Path::Path()
: robosay::base::Path(){
}

Path::~Path(){
	;
}

bool Path::setPath(const void* pParam){
	robosay::base::QA* qa = (robosay::base::QA*)pParam;
	if(!qa){
		return false;
	}
	m_vIndexs.push_back(qa->m_nIdx);
	return true;
}
	
}//namespace general
}//namespace robosay
