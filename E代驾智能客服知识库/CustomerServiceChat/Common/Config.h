#ifndef __ROBOSAY__COMMON__CONFIG__HPP__
#define __ROBOSAY__COMMON__CONFIG__HPP__

#include "Common/libxml2/XmlParser.hpp"
#include "Common/KBConfig.h"
#include "CustomerServiceChat/Common/KBAttribute.h"
#include "SentenceParser/MixParser/Parser.h"
#include "SemanticAnalysis/RuleAnalysis/mean.h"
#include "Common/lib4/CommonDef.h"
#include "SearchEngine/KeywordSearch/BasicSearch.h"

namespace robosay{
using namespace aisms;
class Config : public aisms::KBConfig
{
public:
	 Config()
		: aisms::KBConfig()
		, m_Parser()
		, m_se()
	    , m_Attribute()
	{
	}

bool init(const acl::FileName& sXmlFile, const StringStringSetMap* pWords = NULL, std::string sDicDir = "", std::string sSmtcPath = "",bool needDateConvert=true){
	if(aisms::KBConfig::init(sXmlFile.c_str()) == false){
		return false;
	}

    robosay::XmlParser xml;
	// 使用XML解析器解析XML文件
	if(xml.read(sXmlFile) == false) {
		PRINT("Could not load KB file '"<<sXmlFile<<"'.");
		return false;
	}
	// 读取KB的相关属性
	if(m_Attribute.init(sXmlFile,xml) == false){
        PRINT("Failed to init Attribute node'"<<sXmlFile<<"'.");
		return false;
	}

    std::string sCommonDicDir = "";
    if(sDicDir.empty()){
        sCommonDicDir = sXmlFile.getPath();
    }else{
        sCommonDicDir = sDicDir + "/" + m_Attribute.getKBIndustry();
    }


	// initialize the semantic engine
	if(aisms::KBConfig::has("SemanticSearch") && aisms::KBConfig::isValid("SemanticSearch") == true){
#if defined(_CHAT_LOG)
		ssDebug<<">> Begin to initialize Engine SemanticSearch.";
		acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL0);
#endif
        std::string sSemanticDir;
        if(!sSmtcPath.empty()){
            sSemanticDir = sSmtcPath;
        }else{
            if(sDicDir.empty()){
                sSemanticDir = sXmlFile.getPath();
            }else{
                sSemanticDir = sDicDir + "/" + m_Attribute.getKBIndustry() + "/" + m_Attribute.getKBType();
            }
        }
        std::string sSemanticFile = sSemanticDir + "/" + this->get("SemanticSearch", "semantic_dic");
        PRINT("KB["<<sXmlFile<<"] has loaded semantic ["<<sSemanticFile<<"]\n");
    	if(m_se.init(sSemanticFile, "", pWords) != 0) {
#if defined(_CHAT_LOG)
			ssDebug<<"Module KnowledgeBase initialize failed !";
			acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL1);
#endif
			PRINT("Init SemanticEngine failed !")
			return false;
		}

		/*IVR知识库必须是FAQ类型,并且支持日期且过期时间必须推迟*/
		if(m_Attribute.getKBType() == "FAQ" &&  m_se.IsInExternalTag("IVR")){
			needDateConvert  = true;
			const_cast<mxpsr::DateConverter&>(m_Parser.getDateConverter()).setNeedAdjustDate(false);			
		}

#if defined(_CHAT_LOG)
		ssDebug<<"<< Finished initializing Engine SemanticSearch.";
		acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL0);
#endif
		// initialize the mix parser without wordnetwork
		if(m_Parser.init(sXmlFile,false, sCommonDicDir + "/",needDateConvert) == false){
#if defined(_CHAT_ENGINE_DEBUG) || defined(_CHAT_LOG)
			ssDebug<<"Module MixParser initialize failed !";
			acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL1);
#endif
			PRINT("Init mix parser failed !")
			return false;
		}
		
		m_Parser.getDynamicParser().init("UserDic", m_se.getWordsByTag(""));
#if defined(_CHAT_ENGINE_DEBUG) || defined(_CHAT_LOG)
		ssDebug<<"Module MixParser initialize finished !";
		acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL1);
#endif
	}

    m_bBasicSearch = false;
	if(aisms::KBConfig::has("BasicSearch") && aisms::KBConfig::isValid("BasicSearch") == true){
		// initialize the basic search
		bool bFaq = false;
		if(m_Attribute.getKBType() == "FAQ"){
			bFaq = true;
		}
		
		if(m_basicSearch.init(sXmlFile,sCommonDicDir,bFaq ) == false){
			PRINT("Init basic search engine error!")
			return false;
		}
        m_bBasicSearch = true;
	}

	return true;
}

public:
    bool m_bBasicSearch;
	mxpsr::Parser m_Parser;
	bsrch::BasicSearch m_basicSearch;
	ContextDictionary m_se;
	KBAttribute m_Attribute;
};

}//namespace robosay

#endif //__ROBOSAY__COMMON__CONFIG__HPP__
