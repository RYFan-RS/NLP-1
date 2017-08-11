#include "WebChatResponse.h"
#include "Common/lib5/WHL_LogInstance.hpp"

namespace robosay{
namespace base{

WebChatResponse::WebChatResponse():robosay::base::ChatResponse(){}

WebChatResponse::~WebChatResponse(){}

bool WebChatResponse::init(acl::FileName sFileName){
	if(m_xmlResponse.read(sFileName) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("WebChatResponse::init","[m_xmlResponse.read(sFileName)== false]", sFileName.c_str()));
		return false;
	}
	
	removeCommentNodes(m_xmlResponse.getRootNode("WebPageChat"));
	
		return true;
}

std::string WebChatResponse::generateResponse(const robosay::XmlParser& xmlRequest, const std::string& sMixAnswer){
	Response response = divideResponse(sMixAnswer);
	std::string sXmlResponse;
	
	if(response.isXmlAnswer() == true){
		sXmlResponse = generateByXmlAnswer(xmlRequest,response);
		DLOGGER->print(true, "WebChatResponse::generateResponse [response.isXmlAnswer() == true]\nsMixAnswer[%s]\nsXmlResponse[%d]\n", sMixAnswer.c_str(), sXmlResponse.c_str());
	}else{
		sXmlResponse = generateByTextAnswer(xmlRequest,response);
		DLOGGER->print(true, "WebChatResponse::generateResponse [response.isXmlAnswer() == false]\nsMixAnswer[%s]\nsXmlResponse[%d]\n", sMixAnswer.c_str(), sXmlResponse.c_str());
	}
	
	return sXmlResponse;
}

void WebChatResponse::generateStartStamp(){
	SETVALUE("//WebPageChat/RobotMsg/TimeStamp/RobotReceiveTime",acl::Time::getCurrentTime())
}

void WebChatResponse::generateFinishStamp(){
	SETVALUE("//WebPageChat/RobotMsg/TimeStamp/RobotResponseTime",acl::Time::getCurrentTime())
}

std::string WebChatResponse::generateByXmlAnswer(const robosay::XmlParser& xmlRequest, const Response& response){
	robosay::XmlParser xmlAnswer;
		
	if(xmlAnswer.parse(response.sAnswer) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("WebChatResponse::generateByXmlAnswer","[xmlAnswer.parse(response.sAnswer) == false]", response.sAnswer.c_str()));
		return "";
	}
	
	xmlNodePtr cur = xmlAnswer.getRootNode("WebPageChat");
	copySamePathNodes(cur);
	SETVALUE("//WebPageChat/ChatID",xmlRequest.getXPathValue("//WeChat/ChatID"));
	SETVALUE("//WebPageChat/ChatUserType","0");
	SETVALUE("//WebPageChat/SessionID",xmlRequest.getXPathValue("//WeChat/SessionID"));
	
	SETCDATAVALUE("//WebPageChat/RobotMsg/Context",response.sContext);
	
	std::string sXmlResponse = m_xmlResponse.toStr();
	if(resetNodes() == false){
		ELOGGER->print(true, "WebChatResponse::generateByXmlAnswer [resetNodes() == false]\nRESETNODE ERROR, CHECK THE WECHATRESPONSE!!!\n");
	}
	
	return sXmlResponse;
}

std::string WebChatResponse::generateByTextAnswer(const robosay::XmlParser& xmlRequest, const Response& response){
	robosay::XmlParser xmlAnswer;
		
	// 往特定节点写入信息
	SETVALUE("//WebPageChat/ChatID",xmlRequest.getXPathValue("//ChatInfo/ChatID"))
	SETVALUE("//WebPageChat/ChatUserType","1")
	SETVALUE("//WebPageChat/SessionID",xmlRequest.getXPathValue("//ChatInfo/SessionID"))
	SETVALUE("//WebPageChat/ServiceName","")
	SETVALUE("//WebPageChat/RobotMsg/ViewLayout/MsgType","0")
	SETCDATAVALUE("//WebPageChat/RobotMsg/ViewLayout/MsgContent",response.sAnswer);
		SETCDATAVALUE("//WebPageChat/RobotMsg/Context",response.sContext);
	std::string sXmlResponse = m_xmlResponse.toStr();
	if(resetNodes() == false){
		ELOGGER->print(true, "WebChatResponse::generateByTextAnswer [resetNodes() == false]\nRESETNODE ERROR, CHECK THE WECHATRESPONSE!!!\n");
	}
	
	return sXmlResponse;
}

}//namespace base
}//namespace robosay

