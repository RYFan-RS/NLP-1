#ifndef __ROBOSAY__FAQ_CHAT__KNOWLEDGE_BASE__H__
#define __ROBOSAY__FAQ_CHAT__KNOWLEDGE_BASE__H__
#include "Common/lib4/ConfigFile.hpp"
#include "Common/lib4/FileName.hpp"
#include "CustomerServiceChat/KnowledgeBase.h"
#include "CustomerServiceChat/Entry.h"
#include "CustomerServiceChat/FaqChat/QA.h"
#include "CustomerServiceChat/FaqChat/Input.h"
#include "CustomerServiceChat/Result.h"
namespace robosay{
namespace faq{

class KnowledgeBase : public robosay::base::KnowledgeBase
{
public:
	KnowledgeBase(const int nIdx = 0);
	virtual ~KnowledgeBase();
	virtual bool readKB(robosay::XmlParser& xml);

private:

	virtual void makeEntry();
    virtual bool chat(robosay::base::Input& input,robosay::base::VResult& vRet, const std::string& sWay);
    virtual bool runNewChat(robosay::base::Result* oAsk,robosay::base::VResult& vRet, const std::string& sWay){ return true;}
    virtual bool runOldChat(robosay::base::Result* oAsk,robosay::base::VResult& vRet, const robosay::base::Result* oPre, const std::string& sWay) { return true;}

	virtual robosay::base::Input* createInput(){
		return new robosay::faq::Input;
	}

	std::vector<std::string> getRelateQuestion(const int cur_idx);
	
	virtual void InitIvrKBFlag()
	{
		//增加一个特殊的外部标签，区分是否是IVR知识库
		//此函数判断语义文件中是否包含有外部标签IVR
		m_bIsIvrKB = m_Config.m_se.IsInExternalTag("IVR"); 
	}

	virtual std::string GetSceneInfo()const{ return m_strSceneInfo; }
	
private:
	robosay::base::Entry m_Entry;
	std::vector<robosay::faq::QA> m_vQA;
};

}//namespace faq
}//namespace robosay

#endif //__ROBOSAY__FAQ_CHAT__KNOWLEDGE_BASE__H__
