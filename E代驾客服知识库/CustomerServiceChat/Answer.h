#ifndef __ROBOSAY__BASE_CHAT__ANSWER__H__
#define __ROBOSAY__BASE_CHAT__ANSWER__H__
#include "Common/lib4/String.hpp"
#include "Common/lib4/CommonDef.h"
#include "Common/libxml2/XmlParser.hpp"
#include "Common/UserDicParser.hpp"
#include "Answer_info.hpp"

namespace robosay{
namespace base{

class Answer{
public:
	Answer();

	virtual ~Answer();

	bool init(const robosay::XmlParser& xml, xmlNodePtr qaPtr);

	Answer_info getAnswerInfo(const StringStringMap& mAsk,const StringStringMap* mPre)const;

    std::string getType()const;

	bool init(const std::string& value);
private:
	bool parse(std::string s);

	std::string getAnswer(const StringStringMap& mAsk,
		const StringStringMap* mPre = NULL) const;

	std::string toStr() const;

	void setUDParser(const robosay::UserDicParser* pUDParser){
		m_UDParser = pUDParser;
	}

private:
	std::string m_sAnalyze;
	Answer_info	m_answerInfo;
	const robosay::UserDicParser* m_UDParser;
	std::vector<StringStringPair> m_vVariables;
};

}//namespace base
}//namespace robosay

#endif //__ROBOSAY__BASE_CHAT__ANSWER__H__
