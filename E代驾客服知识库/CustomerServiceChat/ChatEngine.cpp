#include "CustomerServiceChat/ChatEngine.h"

#ifndef PRINT_LOG
#define PRINT_LOG
#endif
#include "Common/lib5/WHL_LogInstance.hpp"

whl::mutex_lock whl::logger::m_lock;
whl::logger* whl::logger::m_instance = NULL;

namespace robosay{
namespace base{

ChatEngine::ChatEngine(){

}

ChatEngine::~ChatEngine(){

}

void CloseParentFD(){
    const int nCnt = sysconf(_SC_OPEN_MAX);
    for(int nIdx = 0; nCnt > nIdx; ++nIdx){
        if(nIdx != STDIN_FILENO && nIdx != STDOUT_FILENO && nIdx != STDERR_FILENO){
            close(nIdx);
        }
    }
}

bool ChatEngine::init(const acl::FileName& sConfigFile) {
	acl::String sRootFile = "";
    CloseParentFD();
	std::cerr<<"Execution path : "<<acl::System::getCurrentPath()<<std::endl;
	std::cerr<<"Use config file: "<<sConfigFile<<std::endl;
	std::cerr<<"Current version: "<<ENGINE_VERSION<<std::endl;
	m_sRootFileName = sConfigFile;
	if(m_vKB.init(sConfigFile) == false){
		LOG_FAILE_CALL_PARAM("ChatEngine::init", "m_vKB.init", sConfigFile.c_str());
		return false;
	}
	std::cerr<<"Initialization finished."<<std::endl;
	return true;
}

// 调用接口
bool ChatEngine::process(const std::string& sInput, bool bNeedAnalysis,  std::string& sChatState, Result_info& resultInfo, const std::string& sWay/* = "web"*/){
	DLOGGER->print(true, "ChatEngine::process Input:[%s]\nChatState:[%s]\n", sInput.c_str(), sChatState.c_str());
	//m_timer.start("ChatEngine");
	resultInfo.init();
	acl::String sRet;
	// 检查输入文是否为标准的UTF8字符
	if(checkInputLegality(sInput,sRet) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("ChatEngine::process", "ChatEngine::checkInputLegality", sInput.c_str()));
        resultInfo.m_answer.set_answer(sRet);
		return false;
	}

	// 处理机器人特殊命令
	if(processSpecialInput(sInput,sRet) == true){
		ELOGGER->print(true, "ChatEngine::process processSpecialInput(sInput,sRet)== true \nin[%s]\nout[%s]\n", sInput.c_str(), sRet.c_str());
		resultInfo.m_answer.set_answer(sRet);
		return false;
	}

	bool bRet = m_vKB.execute(sInput, bNeedAnalysis, sChatState, resultInfo, sWay);
	if(!bRet){
        DLOGGER->print(true, FAILE_CALL_PARAM("ChatEngine::process","m_vKB.execute", sInput.c_str()));
	}
	//std::string sTime = m_timer.toStr("ChatEngine");
	//DLOGGER->print(false, "ChatEngine::process call VKnowledgeBase::execute Cost time [%s]\n", sTime.c_str()));
	resultInfo.m_question.set_source(sInput);
	DLOGGER->print(false, "ChatEngine::process Ouput:[%s]\nChatState:[%s]\n", resultInfo.m_answer.get_answer().c_str(), sChatState.c_str());
	return true;
}

bool ChatEngine::processSpecialInput(const std::string& sIn, acl::String& sRet) const{
	if(!sIn.empty() && sIn[0] != '$'){
		return false;
	}

	if(sIn == "$check_loaded_kb"){
		sRet = "系统当前加载的知识库如下：" + m_vKB.getKBFilenames();
		sRet.removeChars(" \r\n\t");
		sRet.remove("xml");
		DLOGGER->print(false, FAILE_CALL_PARAM("ChatEngine::processSpecialInput","check_loaded_kb", sIn.c_str()));
		return true;
	}

	if(sIn == "$check_kb_version"){
		sRet = "系统当前的知识库版本号为：" + m_vKB.getVersion();
		DLOGGER->print(false, FAILE_CALL_PARAM("ChatEngine::processSpecialInput","check_kb_version", sIn.c_str()));
		return true;
	}

	if(sIn == "$check_engine_version"){
		sRet = "系统当前的引擎版本号为：";
		sRet += ENGINE_VERSION;
		DLOGGER->print(false, FAILE_CALL_PARAM("ChatEngine::processSpecialInput","check_engine_version", sIn.c_str()));
		return true;
	}

	return false;
}

bool ChatEngine::checkInputLegality(const acl::String& sIn, acl::String& sRet) const{
	if(sIn.empty()){
		sRet = "您好，您的输入文为空。";
		ELOGGER->print(true, FAILE_CALL_PARAM("ChatEngine::checkInputLegality","sIn.empty", sRet.c_str()));
		return false;
	}

	StringVector vChar = sIn.toCharactor();
	if(vChar.empty()){
		sRet = "你好，您的输入文有未识别字符。";
		ELOGGER->print(true, FAILE_CALL_PARAM("ChatEngine::checkInputLegality","vChar.empty",sIn.c_str()));
		return false;
	}
	return true;
}

std::string ChatEngine::removeWhiteChars(const acl::String& sIn) const{
	std::string sOut;
	bool bDoFlag = true;
	DLOGGER->print(true, FAILE_CALL_PARAM("ChatEngine::removeWhiteChars","sIn",sIn.c_str()));
	for(size_t i=0; i<sIn.length(); i++){
		if((sIn[i]==' ' || sIn[i]=='\t' || sIn[i]=='\r' || sIn[i]=='\n') && bDoFlag==true){
			continue;
		}else if(sIn[i]=='<'){
			bDoFlag = false;
		}else if(sIn[i]=='>'){
			bDoFlag = true;
		}
		sOut += sIn[i];
	}
	DLOGGER->print(true, FAILE_CALL_PARAM("ChatEngine::removeWhiteChars","sOut",sOut.c_str()));
	return sOut;
}

StringVector ChatEngine::getPromptInfo(const std::string& sIn, int count) const{
	StringVector vResult;
	DLOGGER->print(false, "ChatEngine::getPromptInfo In\nsIn:[%s]\ncount:[%d]\n",sIn.c_str(),count);
	m_vKB.getPromptInfo(sIn, vResult, count);

	return vResult;
}

// 获取语义列表
/*StringVector ChatEngine::getSemanticList(const std::string& sKBName, const std::string& sInput) const{
	DLOGGER->print(false, "ChatEngine::getSemanticList In\nsKBName:[%s]\nInput:[%s]\n",sKBName.c_str(),sInput.c_str());
	return m_vKB.getSemanticList(sKBName, sInput);
}

StringVector ChatEngine::getRelateInfo(const std::string& sIn){
	StringVector vResult;
	DLOGGER->print(false, FAILE_CALL_PARAM("ChatEngine::getRelateInfo ","sIn",sIn.c_str()));
	m_vKB.getRelatedQuestion(sIn, vResult);
	return vResult;
}
std::string ChatEngine::process(const std::string& sInput, const bool bNeedAnalysis, std::string& sChatState, std::string& sAnswerType){
	std::string sXmlName, sScmlName;
	std::string sNeedAnalysis = ((bNeedAnalysis == true)?"TRUE":"FALSE");
	DLOGGER->print(false, "ChatEngine::process sInput:[%s]\nbNeedAnalysis:[%s]\n",sInput.c_str(),sNeedAnalysis.c_str());
	DLOGGER->print(false, "sChatState:[%s]\nsAnswerType:[%s]\n",sChatState.c_str(),sAnswerType.c_str());

	return process(sInput, bNeedAnalysis, sChatState, sAnswerType, sXmlName, sScmlName);
}*/

}//namespace base
}//namespace robosay
