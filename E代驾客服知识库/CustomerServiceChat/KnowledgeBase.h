#ifndef __ROBOSAY__BASE_CHAT__KNOWLEDGE_BASE__H__
#define __ROBOSAY__BASE_CHAT__KNOWLEDGE_BASE__H__
#include "CustomerServiceChat/Common/Config.h"
#include "CustomerServiceChat/KnowledgeBase.h"
#include "Common/json/Json.hpp"
#include "CustomerServiceChat/Result.h"
#include "CustomerServiceChat/Entry.h"
#include "CustomerServiceChat/Input.h"
#include "Common/UserDicParser.hpp"
#include "CustomerServiceChat/WordRecords.h"
#include "Filter.h"

namespace robosay{
namespace base{

class KnowledgeBase{
public:
	int		m_nIdx;

public:
	KnowledgeBase(const std::string& sName,const int nIdx = 0);
	virtual ~KnowledgeBase();
	virtual bool init(const acl::FileName& sXmlFile, std::string sCommonDicDir, const std::string& sSmtcPath);
	virtual bool readKB(robosay::XmlParser& xml) = 0;
	virtual bool  execute(const std::string& sText, bool bNeedAnalysis, robosay::base::VResult& vRet,const std::string& sWay, const robosay::base::VResult* pPre = NULL);
	virtual void makeEntry() = 0;
	virtual bool chat(robosay::base::Input& input,robosay::base::VResult& vRet, const std::string& sWay);
	virtual bool runNewChat(robosay::base::Result* oAsk,robosay::base::VResult& vRet, const std::string& sWay) = 0;
	virtual bool runOldChat(robosay::base::Result* oAsk,robosay::base::VResult& vRet,const robosay::base::Result* oPre, const std::string& sWay) = 0;
	virtual json::Object analyze(const std::string& sInput);
    void getKBNames(std::string& sXmlName, std::string& sScmlName);

	virtual robosay::base::Input* createInput() = 0;
	void insertKBIdx(robosay::base::VResult& vRst);
	virtual StringVector getSemanticList(const std::string& sInput) const;
	const std::string getFileName()const{
		return m_Config.m_Attribute.getFileName();
	}
	virtual const std::string getKBType() const{
		return m_Config.m_Attribute.getKBType();
	}
	virtual bool getPromptInfo(const std::string& sIn, StringVector& vResult, int count) const;

	virtual bool getRelatedQuestion(const std::string& sIn, StringVector& vResult,int iQAIndexOfFinalRet=-1);

    KB_info getKBInfo() const { return m_kbInfo;}
	mxpsr::Parser* getMxParser() {return &(m_Config.m_Parser);}

	std::vector<std::string> getUserDic() { return m_Config.m_se.getWordsByTag(""); }

	//判断是否是IVR扩展的知识库
	bool IsIvrKB()const { return m_bIsIvrKB; }
	
	void SetCode(const std::vector<std::string>& strCode) { if(!IsIvrKB()) return;  
	                                               m_strCode = strCode; }

	virtual std::string GetSceneInfo()const{ return ""; }

	std::string GetKBName()const { return m_kbInfo.get_name(); }

	const CWordRecords& GetWordRecords()const { return m_wordRecords; }
	const Filter& GetFilter()const { return m_filter; }
	
private:
	bool IsNeedfulTag(const std::string& strTag,int& pri,std::string &strSceneInfo)const;
	void setDateTagContentFormat(Words& ws) const;	
	
protected:
	bool readScmlAttribute(const robosay::XmlParser& xml, xmlNodePtr qaPtr);
	virtual void InitIvrKBFlag() {}

	void SetWordRecords(const Words& words);

protected:
	Config 				m_Config;
    std::string         m_sDicDir;
	robosay::UserDicParser m_UDParser;
	KB_info				m_kbInfo;

	//用于扩展业务
	std::vector<std::string> 		m_strCode;//业务码
	bool 				m_bIsIvrKB;//记录是否是IVR知识库
	std::string 		m_strSceneInfo;

	CWordRecords		m_wordRecords;//分词信息记录，主要记录分词过程中改变的词的前后对比信息

	Filter				m_filter;
};

}//namespace base
}//namespace robosay

#endif //__ROBOSAY__BASE_CHAT__KNOWLEDGE_BASE__H__
