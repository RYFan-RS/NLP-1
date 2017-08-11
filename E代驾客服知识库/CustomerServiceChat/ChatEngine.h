#ifndef __ROBOSAY__BASE_CHAT__CHATENGINE__H__
#define __ROBOSAY__BASE_CHAT__CHATENGINE__H__

#include "Common/lib4/ConfigFile.hpp"
#include "Common/lib4/FileName.hpp"
#include "Common/lib4/Time.hpp"
#include "Common/lib4/File.hpp"
#include "Common/lib4/System.hpp"
#include "Common/lib4/CommonDef.h"
#include "CustomerServiceChat/VKnowledgeBase.h"

namespace robosay{
namespace base{

#define ENGINE_VERSION "Robosay_2.0_20140508"

class ChatEngine
{
public:
	ChatEngine();
	~ChatEngine();

	// 初始化接口
	bool init(const acl::FileName& sConfigFile);

	// 对话接口
	//std::string process(const std::string& sInput, const bool bNeedAnalysis, std::string& sChatState, std::string& sAnswerType, std::string& sXmlName, std::string& sScmlName);
	//std::string process(const std::string& sInput, const bool bNeedAnalysis, std::string& sChatState, std::string& sAnswerType);

    bool process(const std::string& sText, bool bNeedAnalysis,  std::string& sChatState, Result_info& resultInfo, const std::string& sWay = "0");

	// 获取语义列表接口
	//StringVector getSemanticList(const std::string& sKBName, const std::string& sInput) const;
	// 输入提示接口
	StringVector getPromptInfo(const std::string& sIn, int count=10) const;
	// 输入相关问题接口
	//StringVector getRelateInfo(const std::string& sIn);

private:
	// 处理特殊输入
	bool processSpecialInput(const std::string& sIn, acl::String& sRet) const;
	// 清楚空白字符
	std::string removeWhiteChars(const acl::String& sIn) const;
	// 检查输入的合法性
	bool checkInputLegality(const acl::String& sIn, acl::String& sRet) const;

private:
	VKnowledgeBase 	m_vKB;
	acl::FileName 	m_sRootFileName;
	acl::Time m_timer;
};

}//namespace base
}//namespace robosay

#endif //__ROBOSAY__BASE_CHAT__CHATENGINE__H_
