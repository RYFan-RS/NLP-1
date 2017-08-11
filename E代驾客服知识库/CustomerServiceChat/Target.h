#ifndef __ROBOSAY__BASE_CHAT__TARGET__H__
#define __ROBOSAY__BASE_CHAT__TARGET__H__
#include "Common/lib4/String.hpp"
#include "Common/lib4/CommonDef.h"

namespace robosay{
namespace base{

class Target{
public:
	std::vector<std::pair<std::string,std::string> > m_vFromTo;
public:
	Target();
	virtual ~Target();

	virtual bool empty() const;
	
	virtual bool parse(const acl::String& s);
	
	virtual std::string toStr() const;
};

}//namespace base 
}//namespace robosay

#endif //__ROBOSAY__BASE_CHAT__TARGET__H__
