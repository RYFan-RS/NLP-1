#ifndef __ROBOSAY__GENERAL_CHAT__INPUT__H__
#define __ROBOSAY__GENERAL_CHAT__INPUT__H__
#include "CustomerServiceChat/Input.h"
#include "CustomerServiceChat/Result.h"
#include "CustomerServiceChat/GeneralChat/Path.h"

namespace robosay{
namespace general{
class Input : public robosay::base::Input{
public:
	Input() : robosay::base::Input(){
	}

	~Input(){
	}

	virtual std::string getName() const{
		return "general";
	}
};

}//namespace general
}//namespace robosay

#endif //__ROBOSAY__GENERAL_CHAT__INPUT__H__
