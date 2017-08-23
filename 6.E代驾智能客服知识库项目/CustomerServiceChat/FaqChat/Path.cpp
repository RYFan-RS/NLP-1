#include "CustomerServiceChat/Path.h"
#include "CustomerServiceChat/FaqChat/Path.h"
#include "CustomerServiceChat/QA.h"

namespace robosay{
namespace faq{

Path::Path()
: robosay::base::Path(){
}

Path::~Path(){
	;
}

bool Path::setPath(const void* pParam){
	if(!pParam){
		return false;
	}
	robosay::base::QA* qa = (robosay::base::QA*)pParam;
	m_vIndexs.push_back(qa->m_nIdx);
	return true;
}
	
}//namespace faq
}//namespace robosay
