#ifndef __ROBOSAY__BASE_CHAT__RESULT__H__
#define __ROBOSAY__BASE_CHAT__RESULT__H__
#include <deque>
#include "Common/lib4/CommonDef.h"

namespace robosay{
namespace base{

class ChatResult : public std::deque<std::string>{

public:
	ChatResult();

	~ChatResult();

	std::string toStr();

	bool fromStr(const std::string& str);

	bool push_front(const std::string& str);
	bool push_back(const std::string& str);
};

}//namespace base
}//namespace robosay

#endif //__ROBOSAY__BASE_CHAT__RESULT__H__
