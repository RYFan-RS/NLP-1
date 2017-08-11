#ifndef __ROBOSAY__GENERAL_CHAT__KNOWLEDGE_BASE__H__
#define __ROBOSAY__GENERAL_CHAT__KNOWLEDGE_BASE__H__
#include "Common/mysql/MySqlDatabase.hpp"

#include "Common/lib4/ConfigFile.hpp"
#include "Common/lib4/FileName.hpp"
#include "Common/lib4/ScopedPtr.h"
#include "CustomerServiceChat/KnowledgeBase.h"
#include "CustomerServiceChat/Entry.h"
#include "CustomerServiceChat/GeneralChat/QA.h"
#include "CustomerServiceChat/GeneralChat/Input.h"
#include "CustomerServiceChat/GeneralChat/Path.h"
#include "CustomerServiceChat/Result.h"
#include "TFIDF.h"
#include "PINYIN.h"
#include "CustomerServiceChat/Common/KeyWordSearch.h"

namespace robosay{
namespace general{
class CTestJuiWei
{
public:
	explicit CTestJuiWei(const std::string& strFileName)
		:m_ofs(strFileName.c_str())
	{
		if (m_ofs) 
		{
			m_ofs << "#DATE=2016-08-12\nRESET" << std::endl;				
		}
	}
	void Output(const std::string& strSemantic, const std::string& strAnswer)
	{
		if (strSemantic.empty() || strAnswer.empty()) 
		{
			return;
		}
		
		if (!m_ofs) { return;}

		((acl::String&)strAnswer).removeChars("\n\r\t ");
		
		m_ofs << strSemantic << '\t';
		m_ofs << "[EXPECT_KEYWORD]"<<strAnswer;
		m_ofs << "\nRESET" << std::endl;		
	}
private:
	std::ofstream m_ofs;
};

	

class KnowledgeBase : public robosay::base::KnowledgeBase
{
public:
	static void setCosMinValue(const std::string& configFileName);	
	KnowledgeBase(const int nIdx = 0);
	virtual ~KnowledgeBase();
	virtual bool init(const acl::FileName& sXmlFile, std::string sCommonDicDir, const std::string& sSmtcPath);
	virtual bool readKB(robosay::XmlParser& xml);
	bool initIDF();
	bool calculateTfidf();
	
	virtual bool execute(const std::string& sText, bool bNeedAnalysis, robosay::base::VResult& vRet, const std::string& sWay, const robosay::base::VResult* pPre = NULL);
	void cutSentenceFromAnswer(const acl::String answer,std::vector<std::string>& vecReturn);
	std::string getAnswerContent(const std::string &sAnswerInfo);
    bool analyseTangPoetry();

	//void getKeyWordsDictionary(std::set<std::string>& dicKeyWords);

    bool execute(const std::string& sText, robosay::base::VResult& vResult, const std::string& sWay, 
							const std::vector<std::vector<TfidfValue> >& vInTfidf, const std::map<int,std::string>& mapIdToWord);

	void GetRandomRelateQuestion(std::vector<std::string>& randomRelateQuestion, int randomNum=5)const;

	bool initFromDB(const std::string& sCommonDicDir,acl::MySQLQuery& mySQLQuery,robosay::KeyWordSearch* pKeyWordSearch,const std::string& idField="id", const std::string& questionField="question");

	bool initDefineTags(const std::string& sCommonDicDir,acl::MySQLQuery& mySQLQuery,const std::string& idField="kb_id", const std::string& TagField="name");

	virtual bool getPromptInfo(const std::string& sIn, StringVector& vResult, int count) const;


private:


	virtual void makeEntry();

	virtual bool chat(robosay::base::Input& input,robosay::base::VResult& vRet, const std::string& sWay);
	virtual bool runNewChat(robosay::base::Result* oAsk,robosay::base::VResult& vRet, const std::string& sWay){ return true;}
    virtual bool runOldChat(robosay::base::Result* oAsk,robosay::base::VResult& vRet, const robosay::base::Result* oPre, const std::string& sWay) { return true;}

	virtual robosay::base::Input* createInput(){
		return new robosay::general::Input;
	}
	virtual robosay::base::Result* createResult(){
		robosay::base::Path* newPath = new robosay::general::Path;
		if(!newPath){
			return NULL;
		}
		return new robosay::base::Result("general",newPath);
	}
	std::vector<std::string> parse(const std::string &sIn);
	//bool initTfidf(std::vector<QA>& vQA,robosay::XmlParser& xml);          // 初始化TFIDF操作，加载数据
	bool initPinyin(std::vector<QA>& vQA,robosay::XmlParser& xml, const std::string&  sCommonDicDir);         // 初始化PINYIN操作，加载数据
	bool searchByLevenshteinDistance(const std::string& sIn, robosay::base::VResult& vResult, const std::string& sWay,
									const robosay::base::VResult* pMatchResult=NULL);     // LD 匹配
	

	bool searchByTFIDF(const std::string& sIn, robosay::base::VResult& vResult, const std::string& sWay);                   // TFIDF 匹配
	bool searchByPinyin(const std::string& sIn, robosay::base::VResult& vResult, const std::string& sWay, std::set<int>& setMatched,
						const robosay::base::VResult* pMatchResult=NULL);                 // Pinyin 匹配
    std::vector<std::string> getRelateQuestion(const int cur_idx, const std::string& sText);
	bool searchByTFIDF(const std::string& sIn, robosay::base::VResult& vResult, const std::string& sWay,  
						const std::vector<std::vector<TfidfValue> >& vInTfidf, std::set<int>& setMatched);
	bool chat(robosay::base::Input& input,robosay::base::VResult& vResult, const std::string& sWay,
				const std::vector<std::vector<TfidfValue> >& vInTfidf, const std::map<int,std::string>& mapIdToWord);

	virtual const std::string getKBType() const{
		return "General";
	}

	bool InitContextDictionary(const std::string& sXmlFile, const std::string& sSmtcPath);
	

	inline bool equalWithLength(const std::string& s1, const std::string& s2, int nLen) const;
	inline void replaceAllQuotations(std::string& s, size_t beg) const;

	bool searchByTags(const std::string& sIn, robosay::base::VResult& vResult, const std::string& sWay, std::set<int>& setMatched);
	bool searchByInvertIndex(const std::string& sIn, robosay::base::VResult& vResult, const std::string& sWay,std::set<int>& setMatched,
									std::map<int,std::string>& highWordMatchQA);

public:
	//callBackFunc : void (*)(int, const std::string&)
	template<class CallBackFunc>
	void ForEach(CallBackFunc callBackFunc)const
	{
		for(size_t i=0; i<m_vQA.size(); i++){
			const std::map<WP_TYPE,std::set<std::string> >& wordProperty = m_vQA[i].getWordProperty();

		    std::map<WP_TYPE,std::set<std::string> >::const_iterator iterProperty;
			for (iterProperty=wordProperty.begin(); iterProperty!=wordProperty.end(); ++iterProperty)
			{
				std::set<std::string>::const_iterator iterWord;
				for (iterWord=iterProperty->second.begin(); iterWord!=iterProperty->second.end(); ++iterWord)
				{
					callBackFunc(iterProperty->first, *iterWord);
				}
			}			
		}

	}

	//test
	void OutputInFile(CTestJuiWei& testJuiWei);
	

private:
	robosay::base::Entry m_Entry;
	std::vector<robosay::general::QA> m_vQA;
	//TFIDF m_TFIDF;
	PINYIN m_PINYIN;

	robosay::KeyWordSearch* m_pKeyWordSearch;
	std::string m_strKBFullFileName;
	//std::map<WP_TYPE,std::set<std::string> >m_wordProperty;
	//aisms::mxpsr::Parser *m_parser;
	//TFIDF* m_pTFIDF;
	static float  m_gCosMinValue;
	bool          m_bDB;//从DB加载知识库
	//aisms::mxpsr::Parser m_mixParser;
    std::map<int,int> m_DBIdToQAId;
	
};

}//namespace general
}//namespace robosay

#endif //__ROBOSAY__GENERAL_CHAT__KNOWLEDGE_BASE__H__
