#ifndef __ROBOSAY_HTTP__CLIENT_H__
#define __ROBOSAY_HTTP__CLIENT_H__
#include "Common/lib4/Definations.hpp"
#include "Common/lib4/Utility.hpp"
#include "Common/lib4/Date.hpp"
#include "Common/curl/HttpClient.hpp"
#include "Common/json/Json.hpp"
#include <map>
#include <vector>
#include <string>
#include <sstream>

namespace robosay{

class HttpClient{
public:

	HttpClient():m_bValid(false){
	}

	bool init(std::string UrlPrefix,std::string UrlMethod,std::string UrlParam){
		if(UrlPrefix.empty())
			return false;
		m_sUrlPrefix = UrlPrefix;
		m_sUrlMethod = UrlMethod;
		m_sUrlParam  = UrlParam;
		m_bValid = true;

		return true;
	}	
	
	std::string execute(const std::string& request,std::vector<std::string>& relateQuestions) const
	{
		CHttpClient httpClient;
		if(!m_bValid){
			std::cout<<"HttpClient::execute["<<"prameter is invalid]."<<std::endl;
			return "";
		}
		std::string sUrl = m_sUrlPrefix + m_sUrlMethod + "?" + m_sUrlParam + "=" + request;
		std::string sResponse;
		
		if(CURLE_OK == httpClient.Get(sUrl,sResponse)){
			return transform(sResponse,relateQuestions);
		}

		return "";
	}
	
private:
	
	
	std::string transform(const std::string& sResponse,std::vector<std::string>& relateQuestions) const{
		std::string sResult;
		json::Object obj;
		
		if(sResponse.empty() || Json::read(sResponse,obj) == false){
			std::cout<<"HttpClient::transform["<<"sResponse.empty or Json::read(sResponse,obj)]."<<std::endl;			
			return "";
		}
		#if 0
		{
			json::Object obj1;
			std::string testStr =  "{\"answer\":\"1et\", \"related\":[\"aaaaa\",\"bbbbbb\",\"we哦买来测试\",\"相关问题12\"]}";
			if(Json::read(testStr,obj1) == true){

				json::Array& jarr =  obj1["related"];
				json::Array::iterator it = jarr.Begin();
				json::Array::const_iterator la = jarr.End();
				for(; la != it; ++it){

					relateQuestions.push_back(Json::toString(*it));
				}

			}

		}	
		#endif
		
		json::Array& jarr =  obj["related"];
		json::Array::iterator it = jarr.Begin();
		json::Array::const_iterator la = jarr.End();
		for(; la != it; ++it){
			relateQuestions.push_back(Json::toString(*it));
		}	
		
		return Json::toString(obj["answer"]);
	}
	
private:
	std::string m_sUrlPrefix;
	std::string m_sUrlMethod;
	std::string m_sUrlParam;
	bool        m_bValid;

};

}//

#endif //__ROBOSAY_HTTP__CLIENT_H__
