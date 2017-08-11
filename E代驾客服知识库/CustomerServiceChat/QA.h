#ifndef __ROBOSAY__BASE_CHAT__QA__H__
#define __ROBOSAY__BASE_CHAT__QA__H__
#include "Common/lib4/FileName.hpp"
#include "Common/lib4/Definations.hpp"
#include "Common/libxml2/XmlParser.hpp"
#include "Common/lib4/CommonDef.h"
#include "CustomerServiceChat/Target.h"
#include "CustomerServiceChat/Result.h"
#include "CustomerServiceChat/Answer.h"
#include "CustomerServiceChat/Path.h"

namespace robosay{
namespace base{

class QA
{
public:
	int						m_nIdx;
	int						m_nId;
	std::pair<std::string, std::string>	m_pairQuestion;
	robosay::base::Target* 	m_target;
	StringStringMap			m_mSemantics;
	std::vector<robosay::base::Answer>	m_vWebAnswer;
	std::vector<robosay::base::Answer>	m_vWecAnswer;
	std::vector<robosay::base::Answer>	m_vIVRAnswer;	
	static const std::string  QA_IVRWay;
public:
	QA(const int nIdx);

	virtual ~QA();

	virtual bool init(const robosay::XmlParser& xml, xmlNodePtr qaPtr, const robosay::UserDicParser* pUDParser = NULL);
	virtual std::string toStr() const;

	virtual bool runNewChat(const robosay::base::Result* oAsk,robosay::base::VResult& vRet, const std::string& sWay) const{ return true;}
	virtual bool runOldChat(const robosay::base::Result* oAsk,robosay::base::VResult& vRet,const robosay::base::Result* oPre, const std::string& sWay) const{ return true;}
	bool match(const robosay::base::Result* oAsk,robosay::base::VResult& vRsts, const std::string& sWay, const StringStringMap* mPreSmtc = NULL) const;
	bool make_answer(robosay::base::VResult& vRsts,const std::string& sWay) const;
	virtual void makeEntry(StringIntSetMap& mSmtcQAIdx) const;
	virtual const QA* getTargetQA(const robosay::base::Target& target) const;

	virtual bool getJumpResult(const robosay::base::Result* oAsk,robosay::base::VResult& vRet,const StringStringMap* mPreSmtc, const robosay::base::Target& target,const std::string& sWay) const;
	virtual robosay::base::Result* createResult() const = 0;
//   Answer_info getAnswerInfo(const StringStringMap& mSmtcWords, const StringStringMap* mPre, std::string& sAnswerType) const;
	Answer_info getAnswerInfo(const StringStringMap& mSmtcWords, const StringStringMap* mPre, const std::string& sWay) const;

	virtual robosay::base::Target* createTarget(){
		return new robosay::base::Target();
	}

	std::string getRelateQuestion()const;
};

}//namespace base
}//namespace robosay

#endif //__ROBOSAY__BASE_CHAT__QA__H__
