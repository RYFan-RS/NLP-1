#include "Answer.h"
#include "Common/lib4/Utility.hpp"
#include "Common/lib5/WHL_LogInstance.hpp"

using namespace robosay::base;

Answer::Answer()
:m_UDParser(NULL){

}

Answer::~Answer(){
	m_vVariables.clear();
}
bool Answer::init(const std::string& value)
{
	if(parse(value) == false){
		ELOGGER->print(true, "QA::init Answer format error!!!\n");
		return false;
	}

	m_answerInfo.set_way("0");
	return true;
}


bool Answer::init(const robosay::XmlParser& xml, xmlNodePtr qaPtr){
	acl::String sAnswer = xml.getValue(qaPtr);
	sAnswer.removeChars("\t\r\n");
	if(parse(xml.getValue(qaPtr)) == false){
		ELOGGER->print(true, "QA::init Answer format error!!!\n");
		return false;
	}
	StringStringSetMap se_Attribute = xml.getAttributes(qaPtr);
	const StringStringSetMap::const_iterator last = se_Attribute.end();
	StringStringSetMap::const_iterator itr = se_Attribute.begin();
	for(; last != itr; ++itr){
		if(itr->first == "analyze"){
			if(!itr->second.empty()){
				m_sAnalyze = *(itr->second.begin());
				StringStringMap mRet = acl::Utility::separate(m_sAnalyze, ";", "--");
				if(mRet.empty()){
					ELOGGER->print(true, "QA::init analyze of answer format error!!!\n");
					return false;
				}
			}
		}else if(itr->first == "way"){
			if(!itr->second.empty()){
				m_answerInfo.set_way(*(itr->second.begin()));
			}
		}else if(itr->first == "id"){
			if(!itr->second.empty()){
				m_answerInfo.set_id(*(itr->second.begin()));
			}
		}else{
			;
		}
	}
	return true;
}

bool Answer::parse(std::string s){

	if(s.empty()){
		ELOGGER->print(true, "Answer::parse Input is Empty");
		return false;
	}
	for(;;){
		int nBegin = -1; //s.find_first_of('[');
		if(nBegin < 0){
			break;
		}
		std::string sVal = s.substr(nBegin + 1);
		int nEnd = sVal.find_first_of(']');
		if(nEnd < 0){
			ELOGGER->print(true, FAILE_CALL_PARAM("Answer::parse ","Input is Error", s.c_str()));
			return false;
		}
		m_vVariables.push_back(std::make_pair(s.substr(0, nBegin), sVal.substr(0, nEnd)));
		s = sVal.substr(nEnd + 1);
	}
	if(!s.empty()){
		m_vVariables.push_back(std::make_pair(s,""));
	}
#if defined(_CHAT_LOG)
	std::vector<StringStringPair>::const_iterator itr = m_vVariables.begin();
	const std::vector<StringStringPair>::const_iterator last = m_vVariables.end();
	for(; last != itr; ++itr){
		std::cout<<"Variable:["<<itr->first<<"]--["<<itr->second<<"]\n";
	}
#endif
	return true;
}

std::string Answer::getAnswer(const StringStringMap& mAsk,const StringStringMap* mPre) const {
	std::string sRet;
	std::vector<StringStringPair>::const_iterator itr = m_vVariables.begin();
	const std::vector<StringStringPair>::const_iterator last = m_vVariables.end();
	for(; last != itr; ++itr){
		sRet += itr->first;
		std::string newName = itr->second;
		if(0 == itr->second.find("ORG")){
			DLOGGER->print(true, FAILE_CALL_PARAM("Answer::getAnswer","[0 == itr->second.find('ORG')]", newName.c_str()));
			newName = newName.substr(3);
		}
		if(!itr->second.empty()){
			StringStringMap::const_iterator itrFind = mAsk.find(newName);
			if(itrFind == mAsk.end()){
				DLOGGER->print(true, FAILE_CALL_PARAM("Answer::getAnswer","newName is not find in mAsk", newName.c_str()));
				if(mPre){
					itrFind = mPre->find(newName);
					if(itrFind != mPre->end()){
                        			std::string sValue = itrFind->second;
						if(m_UDParser){
							m_UDParser->getNameByAlias(newName, sValue);
							sRet += sValue;							
						}
						else
						{
							sRet = sRet + "[" + itr->second + "-" + sValue + "]";
						}
					}
				}
			}else{
                std::string sValue = itrFind->second;
				if(m_UDParser){
					m_UDParser->getNameByAlias(newName, sValue);
					sRet += sValue;					
				}
				else
				{
					sRet = sRet + "[" + itr->second + "-" + sValue + "]";
				}				
			}
		}
		
	}

	if(!m_sAnalyze.empty()){
		sRet += "analyze:" + m_sAnalyze;
	}

	return sRet;
}

Answer_info Answer::getAnswerInfo(const StringStringMap& mAsk,const StringStringMap* mPre)const{
	Answer_info ret = m_answerInfo;
	ret.set_answer(getAnswer(mAsk, mPre));
	return ret;
}

std::string Answer::toStr() const{
	std::stringstream ss;
	return ss.str();
}

std::string Answer::getType()const{
    return m_answerInfo.get_way();
}
