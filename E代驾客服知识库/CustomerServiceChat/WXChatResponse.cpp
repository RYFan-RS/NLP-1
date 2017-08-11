#include "WXChatResponse.h"
#include <string.h>
#include "Common/lib5/WHL_LogInstance.hpp"

namespace robosay{
namespace base{

WXChatResponse::WXChatResponse():robosay::base::ChatResponse(){}
WXChatResponse::~WXChatResponse(){}
bool WXChatResponse::init(acl::FileName sFileName){
	if(m_xmlResponse.read(sFileName) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("WXChatResponse::init","m_xmlResponse.read", sFileName.c_str()));
		return false;
	}

	removeCommentNodes(m_xmlResponse.getRootNode("chat"));
	m_sFileName = sFileName;
	return true;
}

std::string WXChatResponse::generateResponse(const robosay::XmlParser& xmlRequest, const std::string& sContext, const Result_info& resultInfo){
	std::string sXmlResponse;
/*Response response = divideResponse(sMixAnswer);
	if(response.isXmlAnswer() == true){
		sXmlResponse = generateByXmlAnswer(xmlRequest, response, sXmlName, sScmlName);
		DLOGGER->print(true, "WXChatResponse::generateResponse [response.isXmlAnswer() == true]\nsMixAnswer[%s]\nsXmlResponse[%s]\n",sMixAnswer.c_str(), sXmlResponse.c_str());
	}else{
		sXmlResponse = generateByTextAnswer(xmlRequest, response, sXmlName, sScmlName);
		DLOGGER->print(true, "WXChatResponse::generateResponse [response.isXmlAnswer() == false]\nsMixAnswer[%s]\nsXmlResponse[%s]\n", sMixAnswer.c_str(), sXmlResponse.c_str());
	}

    if(!response.sTableAnswer.empty()){
		DLOGGER->print(true, FAILE_CALL_PARAM("WXChatResponse::generateResponse", "response.sTableAnswer.empty", response.sTableAnswer.c_str()));
        size_t uBeg = sXmlResponse.find("<table/>");
        if(uBeg != std::string::npos){
            std::string sRet = sXmlResponse.substr(0, uBeg);
            sRet += response.sTableAnswer;
            sRet += sXmlResponse.substr(uBeg + strlen("<table/>"));
            sXmlResponse = sRet;
        }else{
            uBeg = sXmlResponse.find("<table");
            if(uBeg != std::string::npos){
                std::string sRet = sXmlResponse.substr(0, uBeg);
                sRet += response.sTableAnswer;
                std::string sLeftResponse = sXmlResponse.substr(uBeg + strlen("<table"));
                uBeg = sLeftResponse.find("</table>");
                if(uBeg != std::string::npos){
                    sLeftResponse = sLeftResponse.substr(uBeg + strlen("</table>"));
                    sRet += sLeftResponse;
                    sXmlResponse = sRet;
                }
            }
        }
    }
*/
	return sXmlResponse;
}

/*void WXChatResponse::generateStartStamp(){
	SETVALUE("//WeChat/RobotMsg/TimeStamp/RobotReceiveTime", acl::Time::getCurrentTime());
}

void WXChatResponse::generateFinishStamp(){
	SETVALUE("//WeChat/RobotMsg/TimeStamp/RobotResponseTime", acl::Time::getCurrentTime());
}*/

std::string WXChatResponse::generateByXmlAnswer(const robosay::XmlParser& xmlRequest, const Response& response,const std::string& sXmlName, const std::string& sScmlName){
	robosay::XmlParser xmlAnswer;
    std::string sAnswer = response.sAnswer;
    acl::Utility::stringReplace(sAnswer,"_CDATA_BEG", "<![CDATA[");
    acl::Utility::stringReplace(sAnswer,"_CDATA_END", "]]>");

	if(xmlAnswer.parse(sAnswer) == false){
		ELOGGER->print(true, "WXChatResponse::generateByXmlAnswer [xmlAnswer.parse(sAnswer) == false]\nresponse.sAnswer[%s]\nsAnswer[%s]\n", response.sAnswer.c_str(), sAnswer.c_str());
		return "";
	}
	xmlNodePtr cur = xmlAnswer.getRootNode("WeChat");
	copySamePathNodes(cur);
	SETVALUE("//WeChat/ChatID",xmlRequest.getXPathValue("//WeChat/ChatID"))
	SETVALUE("//WeChat/ChatUserType","1")
	SETVALUE("//WeChat/SessionID",xmlRequest.getXPathValue("//ChatInfo/SessionID"))
	SETVALUE("//WeChat/WeChatParam/MsgType",response.sType)
	SETCDATAVALUE("//WeChat/RobotMsg/Context",response.sContext);
	SETCDATAVALUE("//WeChat/RobotMsg/ScmlName", sScmlName);
	SETCDATAVALUE("//WeChat/RobotMsg/XmlName", sXmlName);
	SETVALUE("//WeChat/RobotMsg/TimeStamp/RobotResponseTime",acl::Time::getCurrentTime())
	std::string sXmlResponse = m_xmlResponse.toStr();
	m_xmlResponse.reload(m_sFileName);
	if(resetNodes() == false){
		ELOGGER->print(true, "WXChatResponse::generateByXmlAnswer [resetNodes() == false]\nRESETNODE ERROR, CHECK THE WECHATRESPONSE!!!\n");
	}

	return sXmlResponse;
}

std::string WXChatResponse::generateByTextAnswer(const robosay::XmlParser& xmlRequest, const Response& response,const std::string& sXmlName, const std::string& sScmlName){
	robosay::XmlParser xmlAnswer;

	// 往特定节点写入信息
	SETVALUE("//WeChat/ChatID",xmlRequest.getXPathValue("//ChatInfo/ChatID"))
	SETVALUE("//WeChat/ChatUserType","1")
	SETVALUE("//WeChat/SessionID",xmlRequest.getXPathValue("//ChatInfo/SessionID"))
	SETVALUE("//WeChat/ServiceName","")
	SETVALUE("//WeChat/WeChatParam/itemCount","1")
	SETVALUE("//WeChat/WeChatParam/item/MsgType",response.sType)
	if(xmlRequest.getXPathValue("//RobotMsg/Type") == "3"){
		DLOGGER->print(true, "WXChatResponse::generateByTextAnswer [xmlRequest.getXPathValue('/RobotMsg/Type') == 3]\n【您 说】[%s]\n【小R说】[%s]\n",xmlRequest.getXPathValue("//RobotMsg/Content").c_str(),response.sAnswer.c_str());
		SETCDATAVALUE("//WeChat/WeChatParam/item/Content","【您 说】"+xmlRequest.getXPathValue("//RobotMsg/Content")+"\n【小蒙说】"+response.sAnswer);
	}else{
		DLOGGER->print(true, FAILE_CALL_PARAM("WXChatResponse::generateByTextAnswer","[xmlRequest.getXPathValue('/RobotMsg/Type') != 3]",response.sAnswer.c_str()));
		SETCDATAVALUE("//WeChat/WeChatParam/item/Content",response.sAnswer);
	}
	SETCDATAVALUE("//WeChat/RobotMsg/Context",response.sContext);
	SETCDATAVALUE("//WeChat/RobotMsg/ScmlName", sScmlName);
	SETCDATAVALUE("//WeChat/RobotMsg/XmlName", sXmlName);
	std::string sXmlResponse = m_xmlResponse.toStr();
	if(resetNodes() == false){
		ELOGGER->print(true, "WXChatResponse::generateByTextAnswer [resetNodes() == false]\nRESETNODE ERROR, CHECK THE WECHATRESPONSE!!!\n");
	}

	return sXmlResponse;
}

}//namespace base
}//namespace robosay
