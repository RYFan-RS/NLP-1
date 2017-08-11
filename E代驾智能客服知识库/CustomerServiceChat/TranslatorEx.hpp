#ifndef __AISMS__TRANSLATOR_EX_H__
#define __AISMS__TRANSLATOR_EX_H__

#include "Translator.h"

#include <map>
#include <vector>
#include <iostream>

namespace aisms
{

class TranslatorEx // this class is in order to analyze the file named by Translation.dic; that generate a map which
                 // record content of the file. for example :　airport 北京 PEK
{
public:
	bool init(const acl::FileName& sXmlFile) {return m_translator.init(sXmlFile);}
	void setMixParserPointer(mxpsr::Parser *pMixParser) {m_translator.setMixParserPointer(pMixParser);}
	
	void translateAnswer(std::string& sAnswer, const StringStringMap& semanticMap){
		if(sAnswer.find("(CALL_EXTERNAL_API") != std::string::npos){
			(void)translateSnarioVars(sAnswer,semanticMap);
			clearAllFliterCondition(sAnswer);
		}
		else
		{
			removeVarType(sAnswer);
		}
	}
	
	void translateSnarioVars(std::string& sAnswer, const StringStringMap& semanticMap){
		size_t nBeg = 0;
		
		while(1){
			nBeg = acl::Utility::findChar(sAnswer,'[',nBeg);
			if(nBeg == std::string::npos) break;
			
			size_t nEnd = acl::Utility::findChar(sAnswer,']',nBeg);
			if(nEnd == std::string::npos) break;
			
			std::string sTypeKey = sAnswer.substr(nBeg+1,nEnd-nBeg-1);
			
			size_t nMid = sTypeKey.find("-");
			if(nMid == std::string::npos) break;
			
			std::string sType = sTypeKey.substr(0,nMid);
			std::string sKey = sTypeKey.substr(nMid+1);
			
			std::string sVal;
			if(isInCallFunction(sAnswer,nBeg)){
				if(sType == "出发地" || sType == "目的地"){
					sVal = m_translator.translate("Airport",sKey);
				}else if(sType == "城市名称"){
					sVal = m_translator.translate("HotelCity",sKey);
				}else if(sType == "日期段"){
					sVal = m_translator.translateWithParser("DateRange",sKey);
					acl::DateString ds;
					if(!sVal.empty()){
						ds.readRange(sVal,";",aisms::gOption.sDate);
						sVal = ds.toStr(",");
					}
				}else if(sType.find("ORG") == 0){
					std::string sNewType = sType.substr(3);
					if(sNewType == "出发地" || sNewType == "目的地"){
						sVal = m_translator.translateOrgName("Airport",sKey);
					}else if(sNewType == "城市名称"){
						sVal = m_translator.translateOrgName("HotelCity",sKey);
					}else{
						sVal = sKey;
					}
				}else{
					sVal = sKey;
				}
			}else{
				sVal = sKey;
			}
			
			if(sVal.empty()){
				sAnswer = sAnswer.substr(0,nBeg) + sKey + sAnswer.substr(nEnd+1);
				nBeg += sKey.length() + 1; 
			}else{
				sAnswer = sAnswer.substr(0,nBeg) + sVal + sAnswer.substr(nEnd+1);
				nBeg += sVal.length();
			}
		}

		m_translator.setMixParserPointer(NULL);
	}	

	std::string TranslateName(const std::string& strDicName, const std::string& strSrc)const
	{
		return m_translator.translate(strDicName, strSrc);
	}
	std::string TranslateOrgName(const std::string& strDicName, const std::string& strSrc)const
	{
		return m_translator.translateOrgName(strDicName, strSrc);
	}
	
	std::string TranslateDateRange(const std::string& strDicName, const std::string& strSrc)const
	{
		std::string sVal = m_translator.translateWithParser(strDicName, strSrc);
		acl::DateString ds;
		if(!sVal.empty())
		{
			ds.readRange(sVal,";",aisms::gOption.sDate);
			sVal = ds.toStr(",");
		}
		
		return sVal;
	}
	
private:
	void removeVarType(std::string& sAnswer) const{
		size_t nBeg = 0;
		
		while(1){
			nBeg = acl::Utility::findChar(sAnswer,'[',nBeg);
			if(nBeg == std::string::npos) break;
			
			size_t nEnd = acl::Utility::findChar(sAnswer,']',nBeg);
			if(nEnd == std::string::npos) break;
			
			std::string sTypeKey = sAnswer.substr(nBeg+1,nEnd-nBeg-1);
			
			size_t nMid = sTypeKey.find("-");
			if(nMid == std::string::npos) break;
			
			std::string sKey = sTypeKey.substr(nMid+1);
			
			sAnswer = sAnswer.substr(0,nBeg) + sKey + sAnswer.substr(nEnd+1);
			nBeg += sKey.length(); 
		}
	}

	bool isInCallFunction(const std::string& sAnswer, size_t nPos) const{
		size_t nBeg = sAnswer.rfind("(CALL_EXTERNAL_API:",nPos);
		size_t nBeg1 = sAnswer.rfind(")",nPos);
		
		// (CALL_EXTERNAL_API:...)...[Key]
		if(nBeg1 != std::string::npos && nBeg1 > nBeg) return false;
		
		size_t nEnd = sAnswer.find(")",nPos);
		size_t nEnd1 = sAnswer.find("(CALL_EXTERNAL_API:",nPos);
		
		// [Key]...(CALL_EXTERNAL_API:...)
		if(nEnd1 != std::string::npos && nEnd1 < nEnd) return false;
		
		// (CALL_EXTERNAL_API:...[Key]...)
		if(nBeg < nPos && nPos < nEnd) return true;
		
		return false;
	}	
	/* (CALL_EXTERNAL_API:Java:DomesticTicketQuery:[出发地],[目的地],[ORG出发地],[ORG目的地],[出发日期],,{AirCompany};{DepartureTime};{A*/


	void clearAllFliterCondition(std::string& sAnswer){
		size_t uBeg=0,uEnd;
		
		while(1){
			uBeg = acl::Utility::findChar(sAnswer,'{',uBeg);
			if(uBeg == std::string::npos) break;
			
			uEnd = acl::Utility::findChar(sAnswer,'}',uBeg);
			if(uEnd == std::string::npos) break;
			
			const std::string sKey = sAnswer.substr(uBeg+1,uEnd-uBeg-1);
			
			// 获取Val的值
			//std::string sVal = smFilter.getValue(sKey);
			//if(sVal.empty()){// 若Val的值为空
				// 对为空的Val进行默认处理
				/*
				if(sKey == "日期"){
					acl::Date date;
					if(aisms::gOption.sDate.empty()){
						sVal = date.getDate();
					}else{
						date.setSpecialDate(aisms::gOption.sDate);
						sVal = date.getSpecialDate();
					}
					sAnswer = sAnswer.substr(0,uBeg) + "{" + sKey + "-" + sVal + "}" + sAnswer.substr(uEnd+1);
					uBeg += sKey.length()+sVal.length()+2;
				}else{
				*/
				sAnswer = sAnswer.substr(0,uBeg) + sAnswer.substr(uEnd+1);
		}/*else{// 若Val的值存在
				sAnswer = sAnswer.substr(0,uBeg) + "{" + sKey + "-" + sVal + "}" + sAnswer.substr(uEnd+1);
				uBeg += sKey.length()+sVal.length()+2;
			}*/
	}

private:
	Translator m_translator;
};

}// namespace aisms
#endif //__AISMS__TRANSLATOR_EX_H__
