#include "Input.h"
#include "Path.h"
#include "Common/lib5/WHL_LogInstance.hpp"
#include "Common/lib4/Utility.hpp"
#include "Common/lib4/StringUtility.hpp"

namespace robosay{
namespace base{

Input::Input(){
	;
}

Input::~Input(){
	;
}

bool Input::parse(const std::string& sCmd){
#if defined(_CHAT_LOG)
	ssDebug<<"<< 待解析的json串为:"<<sCmd;
	acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL0);
#endif
	std::cout<<"<< 待解析的json串为:"<<sCmd<<std::endl;
	//m_sIn = sCmd;
	if(Json::read(sCmd, m_RootObj) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("Input::parse","[Json::read(sCmd, m_RootObj) == false]", sCmd.c_str()));
		return false;
	}

	json::Object::iterator itr = m_RootObj.Begin();
	const json::Object::const_iterator last  = m_RootObj.End();
	for(; last != itr; ++itr){
		const std::string& name = itr->name;
		bool bRet = isValidName(name);
		if(!bRet){
			ELOGGER->print(true, FAILE_CALL_PARAM("Input::parse","[!isValidName(name)]", name.c_str()));
			return false;
		}
		json::Array& jarr = itr->element;
		json::Array::iterator it = jarr.Begin();
		json::Array::const_iterator la = jarr.End();
		for(; la != it; ++it){
			const json::Object& obj = (*it);
			Result* newRst = createResult();
			if(!newRst){
				ELOGGER->print(true, "Input::parse [createResult()] is failed!!!\n");
				return false;
			}
			if(!newRst->parse(obj)){
				ELOGGER->print(true, "Input::parse [!newRst->parse(obj)] is failed!!!\n");
				delete newRst;
				return false;
			}
			m_vResult.push_back(newRst);
		}
	}

	StringStringMap mSmtcWords = m_vResult[0]->getSmtcWords();
	StringStringMap::iterator itrFind = mSmtcWords.find("original");
	if(itrFind != mSmtcWords.end()){
		if(m_vResult.size() > 1 && m_vResult[1]->getAnalyze() != "NULL"){
			std::string strKey = getSemanticTag(m_vResult[1]->getAnalyze(), itrFind->second);
			if(!strKey.empty()) {
				mSmtcWords.insert(std::make_pair(strKey, itrFind->second));
				m_vResult[0]->setAnalyze(m_vResult[1]->getAnalyze());
			}else{
				m_vResult[0]->setAnalyze("NULL");
				m_vResult[1]->setAnalyze("NULL");
			}
		}
		mSmtcWords.erase("original");
		m_vResult[0]->setSmtcWords(mSmtcWords, "");
	}

	return true;
}

std::string Input::getSemanticTag(const acl::String &sSemantic,const std::string& sInput){
	std::string sIn = acl::String(sInput).toHalf();
	ELOGGER->print(true, "in Input::getSemanticTag Input: Before DBC[%s] After DBC[%s]!!!\n", sInput.c_str(), sIn.c_str());
	StringStringMap mRet = acl::Utility::separate(sSemantic, ";", "--");
	StringStringMap::const_iterator itr = mRet.find(sIn);
	if(itr != mRet.end()){
		return itr->second;
	}

	return "";
}

robosay::base::Result* Input::createResult(){
	Path* newPath = new robosay::base::Path();
	if(!newPath){
		ELOGGER->print(true, "Input::createResult [new robosay::base::Path()] is failed!!!\n");
		return NULL;
	}
	return new robosay::base::Result(getName(), newPath);
}

}//namespace base
}//namespace robosay

