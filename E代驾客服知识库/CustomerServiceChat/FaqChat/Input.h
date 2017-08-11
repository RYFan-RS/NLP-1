#ifndef __ROBOSAY__FAQ_CHAT__INPUT__H__
#define __ROBOSAY__FAQ_CHAT__INPUT__H__
#include "CustomerServiceChat/Input.h"
#include "CustomerServiceChat/Result.h"
#include "CustomerServiceChat/FaqChat/Path.h"

namespace robosay{
namespace faq{
class Input : public robosay::base::Input{
public:
	Input() : robosay::base::Input(){
		;
	}

	~Input(){
	}

	virtual std::string getName() const{
		return "FAQ";
	}	


};

}//namespace faq
}//namespace robosay

#endif //__ROBOSAY__FAQ_CHAT__INPUT__H__
