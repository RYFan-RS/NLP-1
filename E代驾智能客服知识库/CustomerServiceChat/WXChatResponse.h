#ifndef __ROBOSAY__BASE_CHAT__CHATRESPONSE__H__
#define __ROBOSAY__BASE_CHAT__CHATRESPONSE__H__

#include "ChatResponse.h"
#include "Result_info.hpp"

namespace robosay{
namespace base{

class ChatResponse : public robosay::base::ChatResponse
{
public:
	ChatResponse();
	virtual ~ChatResponse();
	virtual bool init(acl::FileName sFileName);

	std::string generateResponse(const robosay::XmlParser& xmlRequest, const std::string& sMixAnswer, const std::string& sXmlName, const std::string& sScmlName);
	virtual void generateStartStamp();
	virtual void generateFinishStamp();
private:
	std::string generateByXmlAnswer(const robosay::XmlParser& xmlRequest, const Response& response, const std::string& sXmlName, const std::string& sScmlName);
	std::string generateByTextAnswer(const robosay::XmlParser& xmlRequest, const Response& response, const std::string& sXmlName, const std::string& sScmlName);

private:
	acl::FileName m_sFileName;

};

}//namespace base
}//namespace robosay
#endif //__ROBOSAY__BASE_CHAT__CHATRESPONSE__H__

