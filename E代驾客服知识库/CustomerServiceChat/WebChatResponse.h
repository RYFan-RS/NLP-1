#ifndef __ROBOSAY__BASE_CHAT__WEBCHATRESPONSE__H__
#define __ROBOSAY__BASE_CHAT__WEBCHATRESPONSE__H__

#include "ChatResponse.h"

namespace robosay{
namespace base{

class WebChatResponse : public robosay::base::ChatResponse
{
public:
	WebChatResponse();
	virtual ~WebChatResponse();
	virtual bool init(acl::FileName sFileName);	
	virtual std::string generateResponse(const robosay::XmlParser& xmlRequest, const std::string& sMixAnswer);
	virtual void generateStartStamp();
	virtual void generateFinishStamp();
private:
	std::string generateByXmlAnswer(const robosay::XmlParser& xmlRequest, const Response& response);
	std::string generateByTextAnswer(const robosay::XmlParser& xmlRequest, const Response& response);
};

}//namespace base
}//namespace robosay
#endif //__ROBOSAY__BASE_CHAT__WEBCHATRESPONSE__H__

