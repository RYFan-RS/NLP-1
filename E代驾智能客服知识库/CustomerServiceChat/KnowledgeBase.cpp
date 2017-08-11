#include "CustomerServiceChat/KnowledgeBase.h"
#include "Common/lib4/ScopedPtr.h"
#include "CustomerServiceChat/Path.h"

#define _CHAT_LOG
#include "Common/lib5/WHL_LogInstance.hpp"

namespace robosay{
namespace base{

KnowledgeBase::KnowledgeBase(const std::string& sName, const int nIdx)
	: m_nIdx(nIdx)
	, m_Config(),m_bIsIvrKB(false){
}

KnowledgeBase::~KnowledgeBase(){

}

bool KnowledgeBase::init(const acl::FileName& sXmlFile,std::string sCommonDicDir, const std::string& sSmtcPath){
	robosay::XmlParser xml;
	// 使用XML解析器解析XML文件
	if(xml.read(sXmlFile) == false) {
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::init","xml.read", sXmlFile.c_str()));
		return false;
	}

	// 获取名为scml的根节点
	xmlNodePtr rootNode = xml.getRootNode("scml");
	if(rootNode == NULL){
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::init","xml.getRootNode", sXmlFile.c_str()));
		return false;
	}

	bool bRet = readScmlAttribute(xml, rootNode);
	if(!bRet){
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::init", "KnowledgeBase::readScmlAttribute", sXmlFile.c_str()));
		return false;
	}

	//读取KB的Config节点
	bool needDateConvert=false;
	
	if("2"==m_kbInfo.get_business())
	{
		needDateConvert = true; //商旅业务的知识库支持DateConvert，其他类型的知识库不支持DateConvert
		std::cout<<m_kbInfo.get_name()<<" table support dateConvert."<<std::endl;
	}
	
	if(m_Config.init(sXmlFile, NULL, sCommonDicDir, sSmtcPath,needDateConvert) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::init","m_Config.init", sXmlFile.c_str()));
		return false;
	}

	InitIvrKBFlag();
		
	acl::String sTransDic = xml.getXPathValue("//scml/config/SemanticSearch/translate_dic");
	if(!sTransDic.empty()){
		sTransDic = sXmlFile.getPath() + "/" + sTransDic;
		ELOGGER->print(true, "KnowledgeBase::init : KB [%s] has loaded translate_dic [%s]\n", sXmlFile.c_str(),sTransDic.c_str());
		bRet = m_UDParser.init(sTransDic);
		if(!bRet){
			ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::init","m_UDParser.init", sXmlFile.c_str()));
		}
	}

	if (!m_filter.init(xml)){
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::init","m_filter.init", sXmlFile.c_str()));
		return false;
	}

	bRet = readKB(xml);
	if(!bRet){
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::init","readKB", sXmlFile.c_str()));
		return false;
	}

	makeEntry();
	return true;
}

bool KnowledgeBase::readScmlAttribute(const robosay::XmlParser& xml, xmlNodePtr qaPtr){
	StringStringSetMap se_Attribute = xml.getAttributes(qaPtr);
	const StringStringSetMap::const_iterator last = se_Attribute.end();
	StringStringSetMap::const_iterator itr = se_Attribute.begin();
	for(; last != itr; ++itr){
		if(itr->first == "id"){
			if(!itr->second.empty()){
				m_kbInfo.set_id(*(itr->second.begin()));
			}
		}else if(itr->first == "type"){
			if(!itr->second.empty()){
				m_kbInfo.set_type(*(itr->second.begin()));
			}
		}else if(itr->first == "name"){
			if(!itr->second.empty()){
				m_kbInfo.set_name(*(itr->second.begin()));
			}
		}else if(itr->first == "industry"){
			if(!itr->second.empty()){
				m_kbInfo.set_industry(*(itr->second.begin()));
			}
		}else if(itr->first == "level"){
			if(!itr->second.empty()){
				m_kbInfo.set_level(*(itr->second.begin()));
			}
		}else if(itr->first == "business"){
			if(!itr->second.empty()){
				m_kbInfo.set_business(*(itr->second.begin()));
			}
		}else{
			;
		}
	}
	return true;
}
void KnowledgeBase::setDateTagContentFormat(Words& ws)const{
	if (!IsIvrKB()) { return ; }

    for(int i=0;i<ws.size();i++){
		Word& wordMe = ws.at(i);
		if(wordMe.hasTag("日期")){
			std::string text = wordMe.getWord();
			((acl::String&)text).removeChar('-');
			wordMe.setWord(text);
		}
	}
}
json::Object KnowledgeBase::analyze(const std::string& sInput) {
	// 获取由MixParser的dynamic解析出来的含标签的Words
	Words ws;
	if(!aisms::gOption.sDate.empty()){
		ws = m_Config.m_Parser.dynamicParse(sInput, aisms::gOption.sDate);
	}else{
		ws = m_Config.m_Parser.dynamicParse(sInput);
	}
	
	//setDateTagContentFormat(ws);
	SetWordRecords(ws);
	
	DLOGGER->print(true, "KnowledgeBase::analyze sInput[%s]\n切词结果[%s]\n", sInput.c_str(),ws.toStr().c_str());
	std::cout<<"切词结果："<<ws.toStr()<<std::endl;
	// 使用SAE进行语义分析
	sae::MeanWords words(ws,&(m_Config.m_se.getExcutableData().getDataManager()));
	VMeanResult vMeanResult = m_Config.m_se.analyze("", words, m_Config.m_se.getExecuteMethod("NORMAL_ANALYSIS"));

	m_filter.reset();
	std::map<std::string,std::string> semanticVariable = words.getTagContentMap();
	m_filter.scanFilter(semanticVariable);
	m_filter.scanRange(semanticVariable);
	

	int pri = 0, MaxPri= 9999; 
	std::string  semanticTagWithMaxPri, tagValue = json::String("NULL"),strSceneInfo;

	// 获取句子的语义点
	acl::String sTags = words.getStrTags();
#if defined(_CHAT_LOG)
	vMeanResult.dump();std::cout<<std::endl;

	//std::vector < std :: string > vRuleIdName;
	//vMeanResult.getRuleIdNameVec(vRuleIdName);
	//PRINT("RuleId: "<<acl::debug::toStr(vRuleIdName))
		

	std::cout<<"整句标签为:"<<sTags<<std::endl;
#endif
	StringVector vTags = sTags.separate(",");
	json::Object oCommand;
	json::Array oSmtcArray;

	for(size_t i=0; i<vTags.size(); i++){
		std::string sTag = vTags[i];
		if(!IsNeedfulTag(sTag,pri,strSceneInfo)){ continue; }
		if(pri > 0) {
			if(MaxPri>pri){
				MaxPri = pri;
				semanticTagWithMaxPri = sTag;
				m_strSceneInfo        = strSceneInfo;
			}
			continue;
		}
		
		if(!sTag.empty() && sTag != "日期"){
			json::Object oItem;
			oItem[sTag] = json::UnknownElement(json::String("NULL"));
			oSmtcArray.Insert(oItem);
		}
	}

	// 获取词语的语义点
	std::multimap<std::string, std::string> mSemanticVariable = words.getTagContentMultiMap();
	const std::multimap<std::string, std::string>::const_iterator last = mSemanticVariable.end();
	std::multimap<std::string, std::string>::const_iterator itr = mSemanticVariable.begin();
	for(; last != itr; ++itr){
#if defined(_CHAT_LOG)
		std::cout<<"key="<<itr->first<<"	value="<<itr->second<<std::endl;
#endif
		if(!IsNeedfulTag(itr->first,pri,strSceneInfo)){ continue; }
		if(pri > 0) {
			if(MaxPri>pri){
				MaxPri = pri;
				semanticTagWithMaxPri = itr->first;
				m_strSceneInfo        = strSceneInfo;
				tagValue              = itr->second;
			}
			continue;
		}		
		if(!itr->first.empty()){
			json::Object oItem;
			oItem[itr->first] = json::UnknownElement(itr->second);
			oSmtcArray.Insert(oItem);
		}
	}

	if(!semanticTagWithMaxPri.empty()){
		json::Object oItem;
		oItem[semanticTagWithMaxPri] = json::UnknownElement(tagValue);
		oSmtcArray.Insert(oItem);	
	}
	
	//if(oSmtcArray.Empty())
	{
		json::Object oItem;
		oItem["original"] = json::UnknownElement(sInput);
		oSmtcArray.Insert(oItem);
	}
	{
		json::Object oItem;
		oItem["question"] = json::UnknownElement(sInput);
		oSmtcArray.Insert(oItem);
	}

	oCommand["semantic"] = oSmtcArray;
	oCommand["path"] = json::UnknownElement(json::String("NULL"));
	oCommand["analyze"] = json::UnknownElement(json::String("NULL"));

	return oCommand;
}

bool KnowledgeBase::execute(const std::string& sText, bool bNeedAnalysis, robosay::base::VResult& vResult, const std::string& sWay, const robosay::base::VResult* pPre /*= NULL*/){
	robosay::base::Input* input = createInput();
	scoped_ptr<robosay::base::Input> autoPtrObj(input);
	std::string sCmd = sText;
	m_strSceneInfo = "";
	m_wordRecords.SetQuetion(sText);
	
	if(bNeedAnalysis){
		json::Object tmp = analyze(sText);
		DLOGGER->print(true, "KnowledgeBase::execute [bNeedAnalysis] sText[%s]\n分析完后的json串为:[%s]\n", sText.c_str(),Json::toString(tmp).c_str());
#if defined(_CHAT_LOG)
        std::cout<<"<< 分析完后的json串为:"<<Json::toString(tmp)<<std::endl;
#endif
		json::Array oArray;
		oArray.Insert(json::UnknownElement(tmp));
		if(pPre){
			const robosay::base::VResult::const_iterator last = pPre->end();
			robosay::base::VResult::const_iterator itr = pPre->begin();
			for(; last != itr; ++itr){
				if((*itr)->isValid()){
					std::string sPreJson = (*itr)->toStr();
					size_t uPos = sPreJson.find("<TableXMLAnswer>");
					if(uPos != std::string::npos){
						sPreJson = sPreJson.substr(0, uPos);
					}
					json::Object oPreObj;
					if(Json::read(sPreJson, oPreObj) == false){
						ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::execute","Json::read", sCmd.c_str()));
						vResult.sMessage = "[ERROR]:PreResult的Json串解析错误, 命令为:[" + sCmd + "]";
						return false;
					}
					oArray.Insert(json::UnknownElement(oPreObj));
				}
			}
		}
		json::Object oLine;
		acl::String sKBType = getKBType();
		oLine[sKBType] = oArray;
		sCmd = Json::toString(oLine);
	}

	if(input->parse(sCmd) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::execute","input->parse", sCmd.c_str()));
		vResult.sMessage = "[ERROR]:输入命令解析错误, 命令为:[" + sCmd + "]";
		return false;
	}
	input->m_sIn = sText;

	chat(*input,vResult, sWay);
	if(vResult.empty()){
		ELOGGER->print(true, "KnowledgeBase::execute vResult.empty() : 未找到对应的answer\n");
		vResult.sMessage = "[ERROR]:未找到对应的answer";
		return false;
	}

	return true;
}

bool KnowledgeBase::chat(robosay::base::Input& input,robosay::base::VResult& vResult, const std::string& sWay){
	//依次进行上次对话还原
	int uIdx = input.m_vResult.size() - 1;
	for(; 0 < uIdx; --uIdx){
		robosay::base::VResult vMidRslt;
		runOldChat(input.m_vResult[0],vMidRslt,input.m_vResult[uIdx], sWay);
		if(!vMidRslt.empty()){
			vResult.moveFrom(vMidRslt);
		}
	}
	vResult.sort();
	if(vResult.empty()){// || vResult[0]->m_nScore < 0.5f){
		ELOGGER->print(true, "KnowledgeBase::chat vResult.empty()\n");
		robosay::base::VResult vMidRslt;
		runNewChat(input.m_vResult[0],vMidRslt, sWay);
		if(!vMidRslt.empty()){
			vResult.moveFrom(vMidRslt);
			vResult.sort();
		}
	}

	return true;
}

void KnowledgeBase::insertKBIdx(robosay::base::VResult& vResult){
	const robosay::base::VResult::const_iterator last = vResult.end();
	robosay::base::VResult::iterator itr = vResult.begin();
	for(; last != itr; ++itr){
		if(*itr){
			IntVector& vIdxs = (*itr)->m_pPath->m_vIndexs;
			size_t uCnt = vIdxs.size();
			vIdxs.resize(uCnt + 1);
			for(size_t uIdx = uCnt; uIdx > 0; --uIdx){
				vIdxs[uIdx] = vIdxs[uIdx - 1];
			}
			vIdxs[0] = m_nIdx;
		}
	}
}

// 获取语义列表接口
StringVector KnowledgeBase::getSemanticList(const std::string& sIn) const{
	StringVector vSemantic = m_Config.m_se.getSemantics();
	if(sIn.empty()){
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::getSemanticList","sIn.empty()",sIn.c_str()));
		return vSemantic;
	}else{
		StringVector vRetSemantic;
		for(size_t i=0; i<vSemantic.size(); i++){
			if(vSemantic[i].find(sIn) != std::string::npos){
				vRetSemantic.push_back(vSemantic[i]);
			}
		}
		return vRetSemantic;
	}
}

bool KnowledgeBase::getPromptInfo(const std::string& sIn, StringVector& vResult, int count)const{
	if(m_Config.m_basicSearch.getPromptInfo(sIn, vResult, count) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::getPromptInfo","m_Config.m_basicSearch.getPromptInfo",sIn.c_str()));
		return false;
	}
	return true;
}

bool KnowledgeBase::getRelatedQuestion(const std::string& sIn, StringVector& vResult,int iQAIndexOfFinalRet){
	if(m_Config.m_basicSearch.getRelatedQuestion(sIn, vResult,iQAIndexOfFinalRet) == false){
		//ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::getRelatedQuestion","m_Config.m_basicSearch.getRelatedQuestion",sIn.c_str()));
		//return false;
	}
	return true;
}

void KnowledgeBase::getKBNames(std::string& sXmlName, std::string& sScmlName){
    m_Config.m_Attribute.getKBInfo(sXmlName, sScmlName);
}

//bool true,需要 false,不需要
bool KnowledgeBase::IsNeedfulTag(const std::string& strTag,int& pri,std::string &strSceneInfo)const
{
	pri = 0;
	strSceneInfo = "";
	if (!IsIvrKB()) { return true; }
	
	//当业务码m_strCode空时，说明不需要进行扩展业务处理
	if (m_strCode.empty() || strTag.empty()) { return true;}
	
	std::vector<std::string> tags = ((const acl::String&)strTag).separate("%");
	if ((tags.size()<=1) || (tags.size() > 2)) { return true; }

	//不论是那种格式，tags[0]标示的是和业务码匹配的标识

	size_t length = 0;
	//std::vector<std::string>::const_iterator iter=m_strCode.begin();
	for (length=0; length < m_strCode.size(); length++)
 	{
 		if (m_strCode[length]== tags[1])
 		{
 			strSceneInfo = tags[1];
			pri=length+1;
			return true;
		}
	}
	
	return false;
}

void KnowledgeBase::SetWordRecords(const Words& words)
{
	m_wordRecords.Clear();
	
	Words::const_iterator iter=words.begin();
	for (; iter!=words.end(); ++iter)
	{
		m_wordRecords.AddRecord(iter->getWord(),iter->getOriginalWord());
	}
}

}//namespace base
}//namespace robosay

