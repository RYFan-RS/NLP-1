#ifndef __ROBOSAY__BASE_CHAT__V__KNOWLEDGEBASE__H__
#define __ROBOSAY__BASE_CHAT__V__KNOWLEDGEBASE__H__

#include "Common/lib4/ConfigFile.hpp"
#include "Common/lib4/FileName.hpp"
#include "Common/lib4/Time.hpp"
#include "Common/lib4/CommonDef.h"
#include "CustomerServiceChat/KnowledgeBase.h"
#include "CustomerServiceChat/Common/KBAttribute.h"
#include "CustomerServiceChat/InternalService.h"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include "Common/lib5/WHL_Log.hpp"
#include "CustomerServiceChat/Result_info.hpp"
#include "CustomerServiceChat/TranslatorEx.hpp"
#include "CustomerServiceChat/Common/KeyWordSearch.h"

#include  "Common/simpleTextConvert.hpp"

namespace robosay{

    namespace general{
        class KnowledgeBase;
    }	
}

namespace robosay{
namespace base{

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

typedef struct tag_DefaultAnswer{
	std::string sAnswer;
	std::string sAnalyze;
	std::string sKBPath;
}DefaultAnswer;

class ExternalAPI_info{
public:
	ExternalAPI_info(){
		m_pExtClient = NULL;
	}
	robosay::InternalServiceIf*         m_pExtClient;
	boost::shared_ptr<TSocket>          m_socket;
	boost::shared_ptr<TTransport>       m_transport;
	boost::shared_ptr<TProtocol>        m_protocol;
};

class VKnowledgeBase
{
public:
	VKnowledgeBase();
	~VKnowledgeBase();
	bool init(const acl::FileName& sFileName);

	bool execute(const std::string& sInputText, bool bNeedAnalysis,  std::string& sContext, Result_info& resultInfo, 
		std::vector<std::pair<std::string,std::string> >& resultList);

	const std::string& getVersion() const;
	const std::string getKBFilenames() const;

	// 获取语义列表接口
	StringVector getSemanticList(const std::string& sKBName, const std::string& sInput) const;

	bool getPromptInfo(const std::string& sIn, std::vector<std::string>& vResult, int count) const;

private:
	//bool getRelatedQuestion(const std::string& sIn, std::vector<std::string>& vResult);
    bool getRelatedQuestion(const std::string& sIn, Result_info& resultInfo, const Path* pPathOfFinalRet=NULL,
                            const robosay::base::VResult* pVResult=NULL);

	void sortRelatedQuestion(StringVector& vResult) const;
	KnowledgeBase* createKnowledgeBase(const acl::FileName& sKBPath, const int nIdx, robosay::general::KnowledgeBase*& pGeneral) const;
    std::string talkExternalAPI(const std::string& sText);
    std::string executeExternalAPI(ExternalAPI_info& curFreeChat, const std::string& sCmd);
    int loadSemanticConf(const acl::FileName& sSemanticConf);
	robosay::base::Result* makeDefaultResult(const std::string& sInput);

    bool initMySqlKB(acl::ConfigFile& cfgReader, const acl::FileName& sFileName);
    bool initPersonalKB(acl::ConfigFile& cfgReader, const acl::FileName& sFileName);
    bool initSimpleKB(acl::ConfigFile& cfgReader, const acl::FileName& sFileName);
    bool initActivityListKB(acl::ConfigFile& cfgReader, const acl::FileName& sFileName);
    bool initOldKB(acl::ConfigFile& cfgReader, const acl::FileName& sFileName);
    bool initExternalAPISvr(acl::ConfigFile& cfgReader, const acl::FileName& sFileName);
    bool initFreeChatSvr(acl::ConfigFile& cfgReader, const acl::FileName& sFileName);
    std::string getKBDir(const acl::FileName& xml);
    bool moveUsrDic(const acl::FileName& sDicPath, const acl::FileName& sXmlFile);
	std::string getSmtcPath(const std::string& sXmlName);
	void initTfidf();
	bool IsUseCurKB(robosay::base::KnowledgeBase* pCurKB)const;
	//void ExtractQuestionInfo(const std::string& strInputText, std::string& strQuestion, activity_list::CActivityAreaInfo& activityAreaInfo);
	bool getActivityContent(const std::string& activityName,const std::string& sWay, Result_info& resultInfo,  std::string& sContext);
	void GetActivityInfoIfActivityScene(robosay::base::KnowledgeBase* pCurKB, robosay::base::VResult& vResult);

	bool getSearchResultList(const robosay::base::VResult& vRet, std::vector<std::pair<std::string,std::string> >& resultList);
private:
	acl::FileName m_sRootFileName;                     // 根配置文件的路径
	std::string m_sVersion;                            // 知识库版本号
	std::vector<robosay::base::KnowledgeBase*>  m_vpKB;
	std::vector<robosay::general::KnowledgeBase*>  m_vpGeneralKB;

    StringVector	m_vKBFilename;

	//aisms::TranslatorEx  m_translatorEx;
	std::string 				m_sUnhitHintAnswer;		// 用户连续输入无法匹配时的提示文
	std::vector<DefaultAnswer> 	m_vDefaultAnswers;   	// 用户的问题无法解答时的回答文
	std::string 	m_sGarbledHint;                     // 用户的输入中含有非UTF8字符时的提示文
	int				m_nMaxAnswerCnt;
	StringVector	m_vSameHitHintAnswer;

	std::vector<ExternalAPI_info>		m_vpFreeChat;
	ExternalAPI_info				m_pExternalAPI;

    StringStringMap                m_mSmtcPath;
	acl::String 						    m_sCommonDicDir;
	acl::String						m_sPersonalSmtcPath;

	whl::daily_log						m_UnknowQuestions;

    std::string                             m_sKBDir;

	robosay::KeyWordSearch              m_keyWordSearch;
	std::vector<std::string> 	m_strCode;//IVR扩展功能业务码
	robosay::simpleTextConvert  m_simpleTextConvert;
	
};

}//namespace base
}//namespace robosay

#endif //___ROBOSAY__BASE_CHAT__VKNOWLEDGEBASE__H_

