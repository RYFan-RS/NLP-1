#ifndef __ROBOSAY__FAQ_CHAT__QA__H__
#define __ROBOSAY__FAQ_CHAT__QA__H__

#include "CustomerServiceChat/QA.h"
#include "CustomerServiceChat/FaqChat/Path.h"
#include "CustomerServiceChat/Result.h"

namespace robosay{
namespace faq{

class QA : public robosay::base::QA
{
public:
	QA(const int nIdx)
		: robosay::base::QA(nIdx){
	}

	virtual ~QA(){
	
	}

	virtual robosay::base::Result* createResult() const{
		robosay::faq::Path* pPath = new robosay::faq::Path;
		if(!pPath){
			return NULL;
		}
		return new robosay::base::Result("FAQ", pPath);
	}
};

}//namespace faq
}//namespace robosay

#endif //__ROBOSAY__FAQ_CHAT__QA__H__
