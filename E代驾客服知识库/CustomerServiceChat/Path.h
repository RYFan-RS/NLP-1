#ifndef __ROBOSAY__BASE_CHAT__PATH__H__
#define __ROBOSAY__BASE_CHAT__PATH__H__

#include "Common/lib4/CommonDef.h"

namespace robosay{
namespace base{

class Path {
public:
	IntVector	m_vIndexs;

public:
	Path();

	Path(const Path& rhs);

	virtual ~Path();

	virtual bool setPath(const void* pVal);

	virtual void reset();
	
	virtual bool fromStr(const std::string& sBuf);
	
	virtual bool isValid() const;

	virtual std::string toStr() const;	
	
	int getKBIdx() const;
	int GetQAInx()const;
};

}//namespace BASE
}//namespace robosay

#endif //__ROBOSAY__BASE_CHAT__CHATPATH__H__
