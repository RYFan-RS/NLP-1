#ifndef __ROBOSAY__BASE_CHAT__CHATENGINE__H__
#define __ROBOSAY__BASE_CHAT__CHATENGINE__H__

#include "Common/lib4/ConfigFile.hpp"
#include "Common/lib4/FileName.hpp"
#include "Common/lib4/Time.hpp"
#include "Common/lib4/File.hpp"
#include "Common/lib4/System.hpp"
#include "Common/lib4/CommonDef.h"
#include "CustomerServiceChat/PureSearch/VKnowledgeBase.h"

namespace robosay{
namespace base{

class SearchEngineEx
{
public:
	SearchEngineEx();
	~SearchEngineEx();

	// 初始化接口
	bool init(const std::string& sConfigFile);
	/*first:question  second:answer*/
    bool process(const std::string& sText, std::vector<std::pair<std::string,std::string> >& result, std::string& errorInfo);
	
protected:
	bool checkInputLegality(const acl::String& sIn, acl::String& sRet) const;
	
private:
	VKnowledgeBase 	m_vKB;
};

}//namespace base
}//namespace robosay

#endif //__ROBOSAY__BASE_CHAT__CHATENGINE__H_
