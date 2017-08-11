#include "ChatResponse.h"
#include <string.h>
#include "Common/lib5/WHL_LogInstance.hpp"

namespace robosay{
namespace base{

ChatResponse::ChatResponse(){}
ChatResponse::~ChatResponse() {}

bool ChatResponse::init(const acl::FileName& sFileName){
	if(m_xmlResponse.read(sFileName) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("ChatResponse::init","m_xmlResponse.read", sFileName.c_str()));
		return false;
	}

	removeCommentNodes(m_xmlResponse.getRootNode("chat"));
	m_sFileName = sFileName;
	return true;
}

/*std::string ChatResponse::generateResponse(const robosay::XmlParser& xmlRequest, const std::string& sAnswer, const std::string& sContext, const std::string& sType, const std::string& sXmlName, const std::string& sScmlName){
	std::string sMixAnswer = sAnswer + "<AnswerType>" + sType + "<ChatStateContainer>" + sContext;
	return generateResponse(xmlRequest, sMixAnswer, sXmlName, sScmlName);
}*/

void ChatResponse::copySamePathNodes(xmlNodePtr cur){
	if(cur != NULL){
		cur = cur->xmlChildrenNode;

		while (cur != NULL){
			if(cur ->type == XML_ELEMENT_NODE){
				std::string xPath = (char*)xmlGetNodePath(cur);
				std::string sValue = m_xmlResponse.getValue(cur);
				if(!sValue.empty()){
					xPath = "/" + xPath;
					SETCDATAVALUE(xPath,sValue);
				}
                StringStringSetMap mAttrVals = m_xmlResponse.getAttributes(cur);
                if(!mAttrVals.empty()){
                    m_xmlResponse.setXPathAttributes(xPath, mAttrVals);
                }
			}
			copySamePathNodes(cur);
			cur = cur->next;
		}
	}
	return;
}

void ChatResponse::removeCommentNodes(xmlNodePtr cur){
	if(cur != NULL){
		cur = cur->xmlChildrenNode;
		while (cur != NULL){
			if(cur->type == XML_COMMENT_NODE){
				DLOGGER->print(true, "ChatResponse::removeCommentNodes cur->type == XML_COMMENT_NODE\n");
				xmlNodePtr comment = cur;
				cur = cur->next;
				xmlUnlinkNode(comment);
				xmlFreeNode(comment);
			}else{
				DLOGGER->print(true, "ChatResponse::removeCommentNodes cur->type != XML_COMMENT_NODE\n");
				removeCommentNodes(cur);
				cur = cur->next;
			}
		}
	}
	return;
}

bool ChatResponse::resetNodes(){
	std::set<std::string>::iterator iter = m_sePathContent.begin();
	for(; iter != m_sePathContent.end(); iter++){
		m_xmlResponse.setXPathValue(*iter, "");
	}
	m_sePathContent.clear();
	if(!m_sePathContent.empty()){
		return false;
	}
	return true;
}

std::string ChatResponse::generateResponse(const robosay::XmlParser& xmlRequest, const Result_info& resultInfo){
    robosay::XmlParser xmlParser;

    // chat info
    SETCDATAVALUE("//chat/way", xmlRequest.getXPathValue("//chat/way"));
    SETCDATAVALUE("//chat/message_id", xmlRequest.getXPathValue("//chat/message_id"));
    SETCDATAVALUE("//chat/from_user", xmlRequest.getXPathValue("//chat/to_user"));
    SETCDATAVALUE("//chat/to_user", xmlRequest.getXPathValue("//chat/from_user"));

    //1. KB_info
    std::string str = resultInfo.m_kb.toStr();
    {
        if(xmlParser.parse(str) == false){
            ELOGGER->print(true,  FAILE_CALL_PARAM("ChatResponse::generateResponse", "xmlParser.parse", str.c_str()));
            return "In ChatResponse::generateResponse, Failed to call xmlParser.parse[%s]\n", str.c_str();
        }
        xmlNodePtr cur = xmlParser.getRootNode("chat");
        copySamePathNodes(cur);
    }

    //2. question_info
    str = resultInfo.m_question.toStr();
    {
        if(xmlParser.parse(str) == false){
            ELOGGER->print(true,  FAILE_CALL_PARAM("ChatResponse::generateResponse", "xmlParser.parse", str.c_str()));
            return "In ChatResponse::generateResponse, Failed to call xmlParser.parse[%s]\n", str.c_str();
        }
        xmlNodePtr cur = xmlParser.getRootNode("chat");
        copySamePathNodes(cur);
    }

    //3. robot_info
    str = resultInfo.m_robot.toStr();
    {
        if(xmlParser.parse(str) == false){
            ELOGGER->print(true,  FAILE_CALL_PARAM("ChatResponse::generateResponse", "xmlParser.parse", str.c_str()));
            return "In ChatResponse::generateResponse, Failed to call xmlParser.parse[%s]\n", str.c_str();
        }
        xmlNodePtr cur = xmlParser.getRootNode("chat");
        copySamePathNodes(cur);
    }

    //4. answer_info
    str = resultInfo.m_answer.toStr();
    {
        if(xmlParser.parse(str) == false){
            ELOGGER->print(true,  FAILE_CALL_PARAM("ChatResponse::generateResponse", "xmlParser.parse", str.c_str()));
            return "In ChatResponse::generateResponse, Failed to call xmlParser.parse[%s]\n", str.c_str();
        }
        xmlNodePtr cur = xmlParser.getRootNode("chat");
        copySamePathNodes(cur);
        str = resultInfo.m_answer.answerToXml();
        if(xmlParser.parse(str) == false){
            ELOGGER->print(true,  FAILE_CALL_PARAM("ChatResponse::generateResponse", "xmlParser.parse", str.c_str()));
            return "In ChatResponse::generateResponse, Failed to call xmlParser.parse[%s]\n", str.c_str();
        }
        cur = xmlParser.getRootNode("chat");
        copySamePathNodes(cur);
    }

	std::string sXmlResponse = m_xmlResponse.toStr();
	m_xmlResponse.reload(m_sFileName);
	return  sXmlResponse;
}

bool ChatResponse::SETCDATAVALUE(std::string sPath, std::string sValue){
    if (!m_xmlResponse.getXPath(sPath)){
        std::vector<std::string> vNames = acl::Utility::separate(sPath, "/");
        size_t uCnt = vNames.size();
        size_t uIdx = 0u, uRealCnt = 0u;
        for (; uIdx < uCnt; ++uIdx){
            if(!vNames[uIdx].empty()){
                vNames[uRealCnt++] = vNames[uIdx];
            }
        }
        vNames.resize(uRealCnt);

        std::vector<std::string> vWays;
        vWays.resize(uRealCnt);
        for (uIdx = 0u; uIdx < uRealCnt; ++uIdx){
            vWays[uIdx] = "/";
            for (size_t i = 1u; i < uIdx; ++i){
                vWays[uIdx] += "/" + vNames[i];
            }
        }

        for(uIdx = 0; uIdx < uRealCnt; ++uIdx){
            std::string sPath = vWays[uIdx] + "/" + vNames[uIdx];
            if(!m_xmlResponse.getXPath(sPath)){
                size_t uPosBeg = vNames[uIdx].find("[");
                size_t uPosEnd = vNames[uIdx].find("]");
                std::string sRealName = vNames[uIdx];
                if(uPosBeg > 0 && uPosEnd > uPosBeg ){
                    sRealName = sRealName.substr(0, uPosBeg);
                }
                bool bRet = m_xmlResponse.insertNode(vWays[uIdx], sRealName, "");
                if(!bRet){
                    PRINT("Failed to insert node path"<<vWays[uIdx]<<"], name["<<sRealName<<"]!!!");
                    return false;
                }
            }
        }
    }

    m_xmlResponse.setXPathCDataValue(sPath,sValue);
    m_sePathContent.insert(sPath);
}

}//namespace base
}//namespace robosay

