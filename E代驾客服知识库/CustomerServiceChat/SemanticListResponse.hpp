#ifndef __ROBOSAY__BASE_CHAT__SEMANRICLISTRESPONSE__H__
#define __ROBOSAY__BASE_CHAT__SEMANRICLISTRESPONSE__H__

namespace robosay{
namespace base{
class SemanticListResponse
{
public:
	bool init(acl::FileName sFileName){
		if(m_xmlResponse.read(sFileName) == false){
			PRINT("Read file ["<<sFileName<<"] failed!")
			return false;
		}
		
		m_xmlResponse.deleteCommentNodes(m_xmlResponse.getRootNode("SemanticListResponse"));
		
		return true;
	}
	
	bool SETCDATAVALUE(std::string sPath, std::string sValue){
		
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
	
	std::string genErrorResponse(robosay::XmlParser& xmlRequest, const std::string& sMsg){
		SETVALUE("/SemanticListResponse/SessionID",xmlRequest.getXPathValue("/SemanticListResponse/SessionID"));
		SETVALUE("/SemanticListResponse/SuccessFlag","false");
		SETCDATAVALUE("/SemanticListResponse/Message",sMsg);
		SETVALUE("/SemanticListResponse/Amount","0");
		m_xmlResponse.deleteXPathNode("/SemanticListResponse/Semantics");
		
		std::string sXmlResponse = m_xmlResponse.toStr();
		resetNodes();
		
		return sXmlResponse;
	}
	
	std::string genResponse(robosay::XmlParser& xmlRequest, const std::vector<std::string>& vSemantic){
		SETVALUE("/SemanticListResponse/SessionID",xmlRequest.getXPathValue("/SemanticListResponse/SessionID"));
		SETVALUE("/SemanticListResponse/SuccessFlag","true");
		SETCDATAVALUE("/SemanticListResponse/Message","执行成功!");
		acl::String sAmount(vSemantic.size());
		SETVALUE("/SemanticListResponse/Amount",sAmount);
		m_xmlResponse.deleteXPathNode("/SemanticListResponse/Semantics/Semantic");
        PRINT("Semantic Count:["<<sAmount<<"]\n");
		for(size_t i=0; i<vSemantic.size(); i++){
			m_xmlResponse.insertNode("/SemanticListResponse/Semantics","Semantic",vSemantic[i]);
            PRINT("Semantic["<<i<<vSemantic[i]<<"]\n");
		}

		std::string sXmlResponse = m_xmlResponse.toStr();
		resetNodes();

		return sXmlResponse;
	}

private:
	bool resetNodes(){
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
private:
	robosay::XmlParser m_xmlResponse;
	std::set<std::string> m_sePathContent;
};

}//namespace base
}//namespace robosay
#endif //__ROBOSAY__BASE_CHAT__SEMANRICLISTRESPONSE__H__


