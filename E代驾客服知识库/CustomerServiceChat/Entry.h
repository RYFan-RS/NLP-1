#ifndef __ROBOSAY__BASE_CHAT__ENTRY__H__
#define __ROBOSAY__BASE_CHAT__ENTRY__H__
#include "Common/lib4/String.hpp"
#include "Common/lib4/CommonDef.h"

namespace robosay{
namespace base{

class Entry{
public:
	Entry();

	virtual ~Entry();

	StringIntSetMap& getWordIdxs(){
		return m_mWordIdxs;
	}
	
	IntSet getIdx(const StringVector& vWords);
	
	IntSet getIdx(const StringStringMap& mWords);
	
private:
	StringIntSetMap		m_mWordIdxs;
};

}//namespace base
}//namespace robosay

#endif //__ROBOSAY__BASE_CHAT__ENTRY__H__
