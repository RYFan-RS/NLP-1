#include "Common/mysql/MySqlDatabase.hpp"

#include "CustomerServiceChat/GeneralChat/KnowledgeBase.h"


#include "CustomerServiceChat/Result.h"
#include "CustomerServiceChat/VKnowledgeBase.h"
#include "CustomerServiceChat/ChatResult.h"
#include "CustomerServiceChat/InternalService.h"
#include "CustomerServiceChat/FaqChat/KnowledgeBase.h"



#include "Common/lib5/WHL_LogInstance.hpp"

namespace robosay{
namespace base{

VKnowledgeBase::VKnowledgeBase()
: m_sRootFileName()
, m_sVersion()
, m_vpKB()
, m_vpGeneralKB()
, m_sUnhitHintAnswer()
, m_vDefaultAnswers()
, m_sGarbledHint()
, m_nMaxAnswerCnt(0)
, m_vSameHitHintAnswer()
, m_vpFreeChat()
, m_pExternalAPI()
, m_simpleTextConvert("utf8","utf8"){
}

VKnowledgeBase::~VKnowledgeBase(){
	const std::vector<ExternalAPI_info>::const_iterator last = m_vpFreeChat.end();
	std::vector<ExternalAPI_info>::iterator itr = m_vpFreeChat.begin();
	for(; last != itr; ++itr){
		if(itr->m_pExtClient){
			delete itr->m_pExtClient;
			itr->m_pExtClient = NULL;
		}
	}
	if(m_pExternalAPI.m_pExtClient){
			delete m_pExternalAPI.m_pExtClient;
			m_pExternalAPI.m_pExtClient = NULL;
	}


	for(size_t i=0; i<m_vpKB.size(); i++){
		delete m_vpKB[i];
		m_vpKB[i] = NULL;
	}
	m_vpKB.clear();
}

bool VKnowledgeBase::init(const acl::FileName& sFileName) {
	acl::ConfigFile config;

	// 以#开头的行为注释行，读取该文件
	config.setCommentMarks("#");
	if(config.read(sFileName) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "config.read", sFileName.c_str()));
		return false;
	}
	m_sRootFileName = sFileName;

    std::string sSemanticConf = sFileName.getPath() + "/semantic.conf";
    if(!access(sSemanticConf.c_str(), 0)){
        int nRet = loadSemanticConf(sSemanticConf);
        if(0 > nRet){
			ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "loadSemanticConf", sSemanticConf.c_str()));
        }else{
			ELOGGER->print(true, "SemanticPath has load [%s]\n", sSemanticConf.c_str());
        }
    }

	// 初始化版本号
	m_sVersion = config.getValue("Version");
	if(m_sVersion.empty()){
		ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "m_sVersion.empty()", m_sVersion.c_str()));
		m_sVersion = "unknown version";
	}

	// 初始化未匹配提示回答文
	m_sUnhitHintAnswer = config.getValue("UnhitHintAnswer");
	if(m_sUnhitHintAnswer.empty()){
//		ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "config.getValue", "UnhitHintAnswer"));
	}
	acl::Utility::stringReplace(m_sUnhitHintAnswer, "<BR/>", "\r\n");

    // 初始化未匹配提示回答文
	m_sKBDir = config.getValue("KBDir");
    ELOGGER->print(true, "\tKBDir is [%s]\n", m_sKBDir.c_str());
    
	// 初始化未匹配提示回答文
	m_sPersonalSmtcPath = config.getValue("semanticDir");
    ELOGGER->print(true, "\tsemanticDir is [%s]\n", m_sPersonalSmtcPath.c_str());
	
	// 初始化默认回答文
	acl::String sDefAnswerCnt = config.getValue("DefaultAnswerCount");
	if(sDefAnswerCnt.empty()){
		DefaultAnswer stAnswer;
		stAnswer.sAnswer = config.getValue("DefaultAnswer");
		if(stAnswer.sAnswer.empty()){
			//ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "config.getValue", "DefaultAnswer"));
			stAnswer.sAnswer = "匹配不到答案.";
		}
		acl::Utility::stringReplace(stAnswer.sAnswer, "<BR/>", "\r\n");
		stAnswer.sAnalyze = "";
		stAnswer.sKBPath = "";
		m_vDefaultAnswers.push_back(stAnswer);
	}else{
		int nDefAnswerCnt = sDefAnswerCnt.to<int>();
		char sBuf[100];
		DefaultAnswer stAnswer;
		for(int i = 1; i <= nDefAnswerCnt; ++i){
			sprintf(sBuf, "DefaultAnswer_%d", i);
			stAnswer.sAnswer = config.getValue(sBuf);
			if(stAnswer.sAnswer.empty()){
				ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "config.getValue", sBuf));
				return false;
			}
			acl::Utility::stringReplace(stAnswer.sAnswer, "<BR/>", "\r\n");
			sprintf(sBuf, "DefaultAnalyze_%d", i);
			stAnswer.sAnalyze = config.getValue(sBuf);
			acl::Utility::stringReplace(stAnswer.sAnalyze, "<BR/>", "\r\n");
			sprintf(sBuf, "DefaultPath_%d", i);
			stAnswer.sKBPath = config.getValue(sBuf);
			acl::Utility::stringReplace(stAnswer.sKBPath, "<BR/>", "\r\n");

			m_vDefaultAnswers.push_back(stAnswer);
		}
	}
	if(m_vDefaultAnswers.empty()){
		ELOGGER->print(true, "In VKnowledgeBase::init m_vDefaultAnswers is empty!!!");
		return false;
	}

	// 初始化版本号
	m_sVersion = config.getValue("Version");
	if(m_sVersion.empty()){
		ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "config.getValue", "Version"));
		m_sVersion = "unknown version";
	}

	// 初始化重复提问提示回答文
	const acl::String sSameHitHintAnswer = config.getValue("SameHitHintAnswer");
	if(sSameHitHintAnswer.empty()){
		//ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "config.getValue", "SameHitHintAnswer"));
	}
	m_vSameHitHintAnswer = sSameHitHintAnswer.separate(",");

	// 初始化用户的输入中含有非UTF8字符时的提示文
	/*m_sGarbledHint = config.getValue("GarbledHint");
	if(m_sGarbledHint.empty()){
		ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "config.getValue", "GarbledHint"));
		m_sGarbledHint = "您好，您的输入中存在乱码字符，请检查后重新输入。";
	}*/

	acl::String sAnswerCnt = config.getValue("MaxAnswerCount");
	if(sAnswerCnt.empty()){
		//ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "config.getValue", "MaxAnswerCount"));
		m_nMaxAnswerCnt = 2;
	}else{
		m_nMaxAnswerCnt = sAnswerCnt.to<int>();
		if(m_nMaxAnswerCnt < 1 || m_nMaxAnswerCnt > 10){
			//ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "m_nMaxAnswerCnt < 1 || m_nMaxAnswerCnt > 10", sAnswerCnt.c_str()));
			m_nMaxAnswerCnt = 2;
		}
	}

    m_sCommonDicDir = config.getValue("dicFilePath");
    if(m_sCommonDicDir.empty()){
		ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "m_sCommonDicDir.empty()", sFileName.c_str()));
        m_sCommonDicDir = "../upload/public_data/industry_dic";
    }

	// 初始化publicKB
	#if 0
    bool bRet = initPublicKB(config, sFileName);
    if(!bRet){
        ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "VKnowledgeBase::initPublicKB", sFileName.c_str()));
        return false;
    }
	#endif
    // 初始化personal
    bool bRet = initMySqlKB(config, sFileName);
    if(!bRet){
        ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "VKnowledgeBase::initMySqlKB", sFileName.c_str()));
        return false;
    }

	initTfidf();

	// 获取旧版本的值
    /*bool bRet = initOldKB(config, sFileName);
    if(!bRet){
        ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "VKnowledgeBase::initOldKB", sFileName.c_str()));
        return false;
    }

    if(m_vpKB.empty() && m_vpGeneralKB.empty()){
	      	ELOGGER->print(true,"VKnowledgeBase::init [m_vpKB.empty() && m_vpGeneralKB.empty()]:No knowledge base is initialized successfully!!!\n");
            //return false;
    }*/

	//初始化FreeChat服务
    bRet = initFreeChatSvr(config, sFileName);
    if(!bRet){
        ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "VKnowledgeBase::initFreeChatSvr", sFileName.c_str()));
        return false;
    }

    //初始化ExternalAPI服务
    bRet = initExternalAPISvr(config, sFileName);
    if(!bRet){
        ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "VKnowledgeBase::initExternalAPISvr", sFileName.c_str()));
        return false;
    }



	whl::mkdirs("unknownKey");
	int nRet = m_UnknowQuestions.init("unknownKey", "question");
	if(nRet){
		ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "m_UnknowQuestions.init", "unknown question"));
		return false;
	}

	//显示与用户输入相似的词汇
  	if(!wordDistance.Initialize("./data/word2vec/trainFile.bin")){		
		ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "wordDistance.Initialize", "./data/word2vec/trainFile.bin"));
	}

    return true;
}

bool VKnowledgeBase::initFreeChatSvr(acl::ConfigFile& cfgReader, const acl::FileName& sFileName){
    acl::String sFreeChatSvr = cfgReader.getValue("FreeChatAPISvr");
    if(!sFreeChatSvr.empty()){
		StringVector vIpPorts = sFreeChatSvr.separate(";");
		ExternalAPI_info  curExternalAPI;
		for(size_t uIdx = 0u; uIdx < vIpPorts.size(); ++uIdx){
			acl::String sIPPort = vIpPorts[uIdx];
			StringVector vParams = sIPPort.separate(":");
			if(vParams.size() != 2){
				ELOGGER->print(true, "VKnowledgeBase::init [vParams.size() != 2]: FreeChatAPISvr is invalid, it should be\"IP:Port\"");
				return false;
			}

            std::string sFreeChatIp = vParams[0];
			int nFreeChatPort = acl::String(vParams[1]).to<int>();

			curExternalAPI.m_socket = boost::shared_ptr<TSocket>(new TSocket(sFreeChatIp, nFreeChatPort));
			curExternalAPI.m_transport = boost::shared_ptr<TTransport>(new TBufferedTransport(curExternalAPI.m_socket));
			curExternalAPI.m_protocol = boost::shared_ptr<TProtocol>(new TBinaryProtocol(curExternalAPI.m_transport));
			curExternalAPI.m_pExtClient = new robosay::InternalServiceClient(curExternalAPI.m_protocol);
			if(!curExternalAPI.m_pExtClient){
				ELOGGER->print(true, "VKnowledgeBase::init : Failed to new robosay::InternalServiceClient");
				return false;
			}
			ELOGGER->print(true, "VKnowledgeBase::init : Start to talk to FreeChatAPISvr[%s]\n", sIPPort.c_str());
			std::string sRet = executeExternalAPI(curExternalAPI, "喝奶的好处");
			if(sRet.empty()){
				ELOGGER->print(true, "VKnowledgeBase::init : WARNING, FreeChatAPISvr[%s] is not working, please check!", sIPPort.c_str());
			}
			m_vpFreeChat.push_back(curExternalAPI);
			ELOGGER->print(true, "VKnowledgeBase::init : FreeChatAPISvr[%s] is working\n", sIPPort.c_str());
		}
    }

    return true;
}

bool VKnowledgeBase::initExternalAPISvr(acl::ConfigFile& cfgReader, const acl::FileName& sFileName){
    acl::String sExtAPISvr = cfgReader.getValue("ExternalAPISvr");
    if(!sExtAPISvr.empty()){
		StringVector vParams = sExtAPISvr.separate(":");
		if(vParams.size() != 2){
			ELOGGER->print(true, "VKnowledgeBase::init [vParams.size() != 2]: ExternalAPISvr is invalid, it should be\"IP:Port\"");
			return false;
		}

		std::string sExtAPIIp = vParams[0];
		int nExtAPIPort = acl::String(vParams[1]).to<int>();

		m_pExternalAPI.m_socket = boost::shared_ptr<TSocket>(new TSocket(sExtAPIIp,nExtAPIPort));
		m_pExternalAPI.m_transport = boost::shared_ptr<TTransport>(new TBufferedTransport(m_pExternalAPI.m_socket));
		m_pExternalAPI.m_protocol = boost::shared_ptr<TProtocol>(new TBinaryProtocol(m_pExternalAPI.m_transport));
		m_pExternalAPI.m_pExtClient = new robosay::InternalServiceClient(m_pExternalAPI.m_protocol);
		if(!m_pExternalAPI.m_pExtClient){
			ELOGGER->print(true, "VKnowledgeBase::init : Failed to new robosay::InternalServiceClient");
			return false;
		}
		ELOGGER->print(true, "VKnowledgeBase::init : Start to talk to ExternalAPISvr[%s]\n", sExtAPISvr.c_str());
		std::string sRet = executeExternalAPI(m_pExternalAPI, "CALL_EXTERNAL_API:C++:TimeInquiry:NOW");
		if(sRet.empty()){
			ELOGGER->print(true, "VKnowledgeBase::init : WARNING, ExternalAPI[%s] is not working, please check!", sExtAPISvr.c_str());
		}
		ELOGGER->print(true, "VKnowledgeBase::init : ExternalAPISvr[%s] is working\n", sExtAPISvr.c_str());
    }


    return true;
}

bool VKnowledgeBase::initMySqlKB(acl::ConfigFile& cfgReader, const acl::FileName& sFileName){
    acl::String MySqlInfo = cfgReader.getValue("sqlDataBase");

	
    if(MySqlInfo.empty()){
        ELOGGER->print(true, "sqlDataBase info is not configured.");
        return true;
    }
	acl::String dicFilePath = cfgReader.getValue("dicFilePath");
	if(dicFilePath.empty()){
		ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "dicFilePath is empty", sFileName.c_str()));
		return false;
	}
	
	int nIdx = m_vpKB.size();

	MySqlInfo.removeChars(" \t");
	StringVector vMySql = MySqlInfo.separate(";");
	if(vMySql.size()<8){
        ELOGGER->print(true, "sqlDataBase info number less than 8.");
		return false;
	}

	if(!acl::Utility::isNumber(vMySql[7])){
		ELOGGER->print(true, "sql port info [%s] error.\n", vMySql[7].c_str());	
		return false;
	}

	int portNumber = acl::String(vMySql[7]).to<int>();

	// 初始化各个知识库
	robosay::general::KnowledgeBase* pGeneral = NULL;
	//std::set<std::string> dicKeyWord;
	
	m_keyWordSearch.init(sFileName,dicFilePath); /*初始化*/

	pGeneral = new robosay::general::KnowledgeBase(nIdx);
	if(pGeneral==NULL ){
        ELOGGER->print(true, "VKnowledgeBase::initMySqlKB. pGeneral is NULL");		
		return false;
	}

	acl::MySqlDatabase database;
    if (!database.OPen(vMySql[0], vMySql[1], vMySql[2], vMySql[3],portNumber))
    {
        ELOGGER->print(true, "database.OPen fail.");    
		return false;
    }
	
	acl::MySQLQuery    mySqlQuery;  
	if(!database.QuerySQL("select * from " + vMySql[4], mySqlQuery)){
		ELOGGER->print(true, "open table[%s] fail.\n", vMySql[4].c_str());
		return false;
	}
	
	//查询标题和正文,并对正文建立倒排索引
	pGeneral->initFromDB(dicFilePath, mySqlQuery,&m_keyWordSearch,vMySql[5],vMySql[6]);

	//查询标签
	MySqlInfo = cfgReader.getValue("sqlTagsDataBase");
    if(!MySqlInfo.empty()){
		MySqlInfo.removeChars(" \t");
		StringVector vMySql = MySqlInfo.separate(";");
		while(true){
			if(vMySql.size()<3){
		        ELOGGER->print(true, "TagsDataBase info number less than 3.");
				break;
			}
	
			acl::MySQLQuery    mySqlQuery1;  
			if(!database.QuerySQL("select * from " + vMySql[0], mySqlQuery1)){
				ELOGGER->print(true, "open table[%s] fail.\n", vMySql[0].c_str());
				break;
			}
			pGeneral->initDefineTags(dicFilePath, mySqlQuery1,vMySql[1],vMySql[2]);
			break;
		}
    }
	else{
        ELOGGER->print(true, "TagsDataBase info is not configured.");
	}


	
	m_vpGeneralKB.push_back(pGeneral);

	//m_keyWordSearch.m_errorCorrectionHandle.AddKeywordToDic(dicKeyWord);
    return true;
}

std::string VKnowledgeBase::getSmtcPath(const std::string& sXmlName){
	StringStringMap::const_iterator itrFind = m_mSmtcPath.find(sXmlName);
	std::string sSmtcPath = "";
	if(itrFind != m_mSmtcPath.end()){
		return itrFind->second;
	}
	
	acl::FileName oFileName = m_sPersonalSmtcPath + "/" + sXmlName;
	oFileName = oFileName.getPath();
	if(access(oFileName.c_str() , 0)){
		return "";
	}
	return oFileName;
}

bool VKnowledgeBase::initPersonalKB(acl::ConfigFile& cfgReader, const acl::FileName& sFileName){
     // 获取personalKB的值
    acl::String sKBFiles = cfgReader.getValue("personalKB");
	int errorKBNumber = 0;
    if(sKBFiles.empty()){
        ELOGGER->print(true, "personalKB is disabled");
    }

	robosay::general::KnowledgeBase::setCosMinValue("./data/searchMinValue.txt");

    sKBFiles.removeChars(" \t");
    StringVector vKBFile = sKBFiles.separate(",");

    // 初始化各个知识库
    robosay::general::KnowledgeBase* pGeneral = NULL;
	//std::set<std::string> dicKeyWord;
		
    const size_t uSize = vKBFile.size();
    for(size_t i = 0; i < uSize; ++i){
        const std::string& sKBPath = vKBFile.at(i);
		if(sKBPath.empty()) continue;

		acl::FileName sFileFullPath = getKBDir(sFileName) + "/" + sKBPath;
		
		if(acl::System::isFileExist(sFileFullPath) == false){
			ELOGGER->print(true, "initPersonalKB:KB[%s] is not exist.warnning.\n", sFileFullPath.c_str());
			continue;
		}

		acl::FileName sUsrFullPath = sFileName.getPath() + "/" + sKBPath;
		moveUsrDic(sUsrFullPath, sFileFullPath);
		ELOGGER->print(true, "Start to load KB[%s]\n", sFileFullPath.c_str());
		KnowledgeBase* pKB = createKnowledgeBase(sFileFullPath, m_vpKB.size(), pGeneral);
		if(!pKB){
			ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "createKnowledgeBase", sFileFullPath.c_str()));
			return false;
		}

		if(pKB->init(sFileFullPath, m_sCommonDicDir, getSmtcPath(sKBPath)) == false) {
			ELOGGER->print(true, "KB[%s] has error(s).please check it.\n\n", sFileFullPath.c_str());
			//ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "pKB->init", sFileFullPath.c_str()));
			delete pKB;
			if(pGeneral) {delete pGeneral;}
			errorKBNumber++;
			continue;
			//return false;
		}
		
		if(pKB->getKBType() == "FAQ"){
			m_keyWordSearch.init(sFileFullPath,m_sCommonDicDir); /*初始化*/
		}
		
		m_vpKB.push_back(pKB);
		m_vKBFilename.push_back(sFileFullPath.getNameWithoutPath());
		if(pGeneral){
			if(pGeneral->init(sFileFullPath, m_sCommonDicDir, getSmtcPath(sKBPath)) == false) {
				ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::init", "pGeneral->init", sFileFullPath.c_str()));
				delete pGeneral;
			}else{
				m_vpGeneralKB.push_back(pGeneral);
				//pGeneral->getKeyWordsDictionary(dicKeyWord);
				//pGeneral->ForEach(bind(&general::CTfIdfRemend::AddWord,&m_keyWordSearch.m_TFIDF.m_tfIdfRemend,_2,_1));
			}
			pGeneral = NULL;
		}
	}

	//i
	//m_keyWordSearch.addWords(dicKeyWord);
	//m_keyWordSearch.m_errorCorrectionHandle.AddKeywordToDic(dicKeyWord);
	std::cout<<std::endl;
	if(errorKBNumber!=0){
		ELOGGER->print(true, " %d KBs in all %d KBs has error(s).please check above detail logs.\n\n", errorKBNumber,m_vpKB.size()+errorKBNumber);		
	}
	else{
		ELOGGER->print(true, "all %d KBs are loaded successfully\n\n",m_vpKB.size());		
	}

    return true;
}

void VKnowledgeBase::initTfidf(){
	std::vector<robosay::general::KnowledgeBase*>::iterator it = m_vpGeneralKB.begin();

	//m_keyWordSearch.generateDictionary();

    /*所有知识库一块进行IDF统计*/
	for(;it!=m_vpGeneralKB.end();it++){
		(*it)->initIDF();
	}

	it = m_vpGeneralKB.begin();

	/*IDF统计完然后对TF*IDF进行统计*/
	for(;it!=m_vpGeneralKB.end();it++){
		(*it)->calculateTfidf();
	}

	//initialize TfIdfRemend 
	m_keyWordSearch.m_TFIDF.InitTfIdfRemend();

	#if 0
	std::ofstream ofsOutputConflictWordInfos("WordPropertyConflictInfo.txt");
	m_keyWordSearch.m_TFIDF.m_tfIdfRemend.OutputConflictWordInfos(ofsOutputConflictWordInfos);
	
	//test OutputWordNumber
	m_keyWordSearch.TestOutputWordNumber();

	CTestJuiWei testJuiWei("in.txt");
	it = m_vpGeneralKB.begin();
	for(;it!=m_vpGeneralKB.end();it++)
	{
		(*it)->OutputInFile(testJuiWei);
	}	
	#endif
}

std::string VKnowledgeBase::talkExternalAPI(const std::string& sText){
    std::string sAnswer;
    for(size_t uIdx = 0u; uIdx < m_vpFreeChat.size(); ++uIdx){
		const ExternalAPI_info curFreeChat = m_vpFreeChat[uIdx];
        curFreeChat.m_transport->open();
        curFreeChat.m_pExtClient->process(sAnswer, sText);
        curFreeChat.m_transport->close();
        if(!sAnswer.empty() && sAnswer.find("ERROR") == std::string::npos){
            return sAnswer;
        }
    }

    return "";
}

std::string VKnowledgeBase::executeExternalAPI(ExternalAPI_info& curFreeChat, const std::string& sCmd){
    if(!curFreeChat.m_pExtClient){
        return "";
    }
    std::string sAnswer;
    curFreeChat.m_transport->open();
    curFreeChat.m_pExtClient->process(sAnswer, sCmd);
    curFreeChat.m_transport->close();
    if(!sAnswer.empty() && sAnswer.find("ERROR") == std::string::npos){
        return sAnswer;
    }

    return "";
}

bool VKnowledgeBase::moveUsrDic(const acl::FileName& sDicPath, const acl::FileName& sXmlFile){
    std::string sDicDir = sDicPath.getPath();
    std::string sXmlDir = sXmlFile.getPath();
    char cmd[5212];
    sprintf(cmd, "cp %s/* %s -r",  sDicDir.c_str(), sXmlDir.c_str());
    ELOGGER->print(true, "Execute cmd[%s]\n", cmd);
    system(cmd);
    return true;
}

KnowledgeBase* VKnowledgeBase::createKnowledgeBase(const acl::FileName& sXmlFile, const int nIdx, robosay::general::KnowledgeBase*& pGeneral) const{
	robosay::XmlParser xml;
	// 使用XML解析器解析XML文件
	if(xml.read(sXmlFile) == false) {
		ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::createKnowledgeBase", "xml.read", sXmlFile.c_str()));
		return NULL;
	}

	// 读取KB的相关属性
	robosay::KBAttribute kbAttr;
	if(kbAttr.init(sXmlFile,xml) == false){
		return NULL;
	}

	std::string sType = kbAttr.getKBType();
	pGeneral = NULL;
	if(sType == "FAQ"){
		pGeneral = new robosay::general::KnowledgeBase(nIdx);
		return new robosay::faq::KnowledgeBase(nIdx);
	}else{
		return NULL;
	}
}

typedef struct tag_resultSortMatrix{
	int nScore;
	int nIdx;
	robosay::base::VResult vRslts;
}ResultSortMatrix;


bool VKnowledgeBase::execute(const std::string& sInputText, bool bNeedAnalysis,  std::string& sContext, Result_info& resultInfo, const std::string& sWay){
	std::string sText = sInputText;
	std::string destConvert;
	if(m_simpleTextConvert.cht2chs(sText, destConvert)) {
		sText = destConvert;
	}

	m_keyWordSearch.m_errorCorrectionHandle.CorrectError(sText);

	ChatResult cRst;
	cRst.fromStr(sContext);
	robosay::base::VResult vRet;
	
	ResultSortMatrix topResultMatrix;
    std::vector<robosay::general::KnowledgeBase*>::const_iterator itrGeneral = m_vpGeneralKB.begin();
    std::vector<robosay::general::KnowledgeBase*>::const_iterator lastGeneral = m_vpGeneralKB.end();

	acl::String strInputText(sText);
	strInputText.toUpper();

	std::vector<std::vector<TfidfValue> > tfIdfVec;
	std::vector<std::string> words;
	std::map<int,std::string> mapIdToWord;
	//const std::vector<TfidfValue>& vInTfidf = m_keyWordSearch.getQuestionTfIdf(strInputText);
	if(itrGeneral!=lastGeneral){
		std::map<std::string,std::vector<std::string> >subSentence;
		
		words = (*itrGeneral)->getMxParser()->parseWithJieba(strInputText,subSentence).getWords();


		std::cout<<std::endl;
		for(int tempI=0;tempI<words.size();tempI++){
			std::cout<<" "<<tempI+1<<":"<<words[tempI];
		}
		m_keyWordSearch.filterStopWord(words);		
		std::cout<<"  粗分结束"<<std::endl;
		//m_keyWordSearch.filterStopWord(words);
		mapIdToWord = m_keyWordSearch.getQuestionTfIdfValue(tfIdfVec,words,subSentence);
		
	}

    for(int i = 0; lastGeneral != itrGeneral; ++itrGeneral, ++i){
		if (!IsUseCurKB(*itrGeneral)) { continue; }
        robosay::base::VResult vMidRslt;
        bool bRet = (*itrGeneral)->execute(sText, vMidRslt, sWay,tfIdfVec,mapIdToWord);
        if(bRet && !vMidRslt.empty()){
            ELOGGER->print(true, "Succeed to match general knowledgebase[%s]\n", (*itrGeneral)->getFileName().c_str());
			topResultMatrix.vRslts.moveFrom(vMidRslt);
			/*
			if(topResultMatrix.nScore < vMidRslt[0]->getScore()){
				topResultMatrix.nScore = vMidRslt[0]->getScore();
				topResultMatrix.nIdx = i;
				topResultMatrix.vRslts.clear();  //只保存分数最高的答案
				topResultMatrix.vRslts.moveFrom(vMidRslt);
			}
			*/
        }else{
            ELOGGER->print(true, "Failed to match general knowledgebase[%s]\n",(*itrGeneral)->getFileName().c_str());
        }
    }
	
	if(topResultMatrix.vRslts.size()!=0){
		vRet.moveFrom(topResultMatrix.vRslts);
		//vRet.sort();

		getRelatedQuestionEx(words,resultInfo);
		//getRelatedQuestion(sText, resultInfo,vRet[0]->m_pPath,&vRet);
		vRet.reduceSize(robosay::base::Result::max_result_number);	
		if(vRet[0]->m_pPath->getKBIdx()!=-1){
			vRet.setKBType("FAQ");
			resultInfo.m_kb = m_vpGeneralKB[vRet[0]->m_pPath->getKBIdx()]->getKBInfo();
		}
		
		vRet.ClearPathInfo();		
		//cRst.push_front(vRet.toStr());
		//sContext = cRst.toStr();
		return vRet.getResultInfo(sContext, resultInfo,mapIdToWord);
	}


	vRet.push_back(makeDefaultResult(sText));
	//cRst.push_front(vRet.toStr());
	//m_UnknowQuestions.print(true, "[%s]\n", sText.c_str());
    resultInfo.m_answer   = vRet[0]->getAnswerInfo();
	resultInfo.m_question = vRet[0]->getQuestionInfo();
	return true;
}	
void VKnowledgeBase::getRelatedQuestionEx(const std::vector<std::string>& wordsSrc,Result_info& resultInfo,
															const int number){
	std::vector<std::string>                   wordVec1;
	std::vector<std::string>                   wordVec2;
	int total = 0;
	std::vector<std::string>   words = m_keyWordSearch.getKeyWords(wordsSrc);
	for(int i=0;i<words.size(); i++){
		std::vector<std::string> result = wordDistance.getWordDistance(words[i]); 

		if(result.size()==0)
			continue;

		std::vector<std::string> sOutput;
		std::map<int,std::string> mapIdToWord;
		std::vector<int> vecWordId = m_keyWordSearch.m_TFIDF.transWords(result, mapIdToWord);
		for(int a=0;a<vecWordId.size();a++){
			std::map<int,std::string>::const_iterator itr = mapIdToWord.find(vecWordId[a]);
			if(itr!=mapIdToWord.end()){
				sOutput.push_back(itr->second);
			}
		}
		
 	    for (int a = 0,b=0; a < result.size(); a++){
			if(b<sOutput.size()){
				if(sOutput[b]==result[a]){
					b++;				
				}else{
				
				}
			}else{
				sOutput.push_back(result[a]);
				b=sOutput.size()+1;
			}
    	}

		if(wordVec1.size()==0){
			wordVec1.swap(sOutput);
		}else if(wordVec2.size()==0){
			wordVec2.swap(sOutput);			
		}
	}

	std::vector<std::string> relatedResult ;

	total = wordVec2.size();
	if(wordVec1.size()>total){
		total = wordVec1.size();
	}

	if(total > number){
		total = number;
	}
	for(int i=0;i<total;i++){
		std::string relatedQuestion;
		if(i<wordVec1.size()){
			relatedQuestion+=wordVec1[i];
		}
		if(i<wordVec2.size()){
			if(i<wordVec1.size())
				relatedQuestion+=" ";
			relatedQuestion+=wordVec2[i];
		}	

		relatedResult.push_back(relatedQuestion);
	}
	resultInfo.m_relate.set_questions(relatedResult);
}

robosay::base::Result* VKnowledgeBase::makeDefaultResult(const std::string& sInput){
	robosay::base::Result* pRst =  robosay::base::createResult("FAQ");
	if(!pRst){
		ELOGGER->print(true, "VKnowledgeBase::makeDefaultResult [robosay::base::createResult] is failed!!!\n");
		return NULL;
	}


	/*
	srand(time(NULL));
	int nIdx = rand() % m_vDefaultAnswers.size();
	acl::String sAnswer = m_vDefaultAnswers[nIdx].sAnswer;
	sAnswer.replace("[question]", sInput);
	*/
	pRst->setAnswer("0", "0", "0");
	pRst->setAnalyze("");
	//pRst->m_pPath->fromStr(m_vDefaultAnswers[nIdx].sKBPath);
	pRst->insertSmtcWord("DefaultAnswer", "DefaultAnswer", "0");
	pRst->setQuestionType(Question_info::DefaultAnwser);		
	return pRst;
}


const std::string& VKnowledgeBase::getVersion() const{
	return m_sVersion;
}

const std::string VKnowledgeBase::getKBFilenames() const{
	std::string sRet;

	for(size_t i=0; i<m_vKBFilename.size(); i++){
		if(i != 0) sRet += ",";
		sRet += m_vKBFilename[i];
	}

	return sRet;
}

// 获取语义列表接口
StringVector VKnowledgeBase::getSemanticList(const std::string& sKBName, const std::string& sInput) const{
	StringVector vEmpty;

	for(size_t i=0; i<m_vpKB.size(); i++){
		if(m_vpKB[i]->getFileName().find(sKBName) != std::string::npos){
			return m_vpKB[i]->getSemanticList(sInput);
		}
	}
	
	return vEmpty;
}

bool VKnowledgeBase::getPromptInfo(const std::string& sIn, StringVector& vResult, int count) const{
	for(int i=0; i < m_vpKB.size(); i++){
		m_vpKB[i]->getPromptInfo(sIn,vResult,count);
		if(vResult.size() > count) break;
	}

	for(int i=0; i < m_vpGeneralKB.size(); i++){
		m_vpGeneralKB[i]->getPromptInfo(sIn,vResult,count);
		if(vResult.size() > count) break;
	}
	
	return true;
}

bool VKnowledgeBase::getRelatedQuestion(const std::string& sIn, Result_info& resultInfo, const Path* pPathOfFinalRet,
										const robosay::base::VResult* pVResult){
	int iKbIndex = -1, iQaIndex = -1;
	if (NULL != pPathOfFinalRet)
	{
		iKbIndex = pPathOfFinalRet->getKBIdx();
		iQaIndex = pPathOfFinalRet->GetQAInx();
	}
    if(resultInfo.m_relate.get_questions().empty()){
        StringVector vResult;
		StringVector similarQuestion;
		if(pVResult!=NULL){  //把搜索得分第二第三第四加入到相关问题中，第一得分作为问题的答案
			if(pVResult->size()>1){
				similarQuestion.push_back(pVResult->at(1)->getTarget());
				std::cout<<"the second question is "<<pVResult->at(1)->getTarget()<<std::endl;
			}
			if(pVResult->size()>2){
				similarQuestion.push_back(pVResult->at(2)->getTarget());
				std::cout<<"the 3 question is "<<pVResult->at(2)->getTarget()<<std::endl;				
			}	
			if(pVResult->size()>3){
				similarQuestion.push_back(pVResult->at(3)->getTarget());
				std::cout<<"the 4 question is "<<pVResult->at(3)->getTarget()<<std::endl;				
			}					
		}
		
        for(size_t i = 0; i < m_vpKB.size(); i++){
			if(m_vpKB[i]->getKBType() == "FAQ"){
				int iTmpQaIndex = iKbIndex==i ? iQaIndex : -1;
            	m_vpKB[i]->getRelatedQuestion(sIn, vResult,iTmpQaIndex);
			}
        }
        sortRelatedQuestion(vResult);

		std::vector<std::string>::iterator iter;
		iter = std::find(vResult.begin(), vResult.end(), sIn);
		if(iter != vResult.end()){
			vResult.erase(iter);
		}

		if(vResult.size()>10){
			vResult.resize(10);
		}

		StringVector::const_iterator itr = similarQuestion.begin();
		for(;itr!=similarQuestion.end();itr++){
			StringVector::iterator iterFind = std::find(vResult.begin(), vResult.end(), *itr);
			if(iterFind != vResult.end()){
				vResult.erase(iterFind);
			}
		}

		if(similarQuestion.size()!=0)
			vResult.insert(vResult.begin(), similarQuestion.begin(), similarQuestion.end());

		if(vResult.size()>10){
			vResult.resize(10);
		}
		
        resultInfo.m_relate.set_questions(vResult);
    }

    return true;
}

void VKnowledgeBase::sortRelatedQuestion(StringVector& vResult) const{
	if(!vResult.empty()){
		
		robosay::base::CSortManager<std::string>  sortManager;
		
		int nMaxScore = -1,nMaxScoreIdx = -1;
		StringVector vNewResult;
		IntIntMap::const_reverse_iterator riter;

		for(size_t i=0; i<vResult.size(); i++){
			int nPos = vResult[i].find(":");
			if(nPos == std::string::npos || nPos > 32 || nPos < 0) continue;

			acl::String sScore(vResult[i].substr(0,nPos));
			vResult[i] = vResult[i].substr(nPos+1);

			int nScore = sScore.to<int>();

			sortManager.Add(nScore,vResult[i]);
		}

		sortManager.Sort();

		const std::vector< robosay::base::CSortManager<std::string>::NodeElem >& elems = sortManager.GetAllElems();
		
		std::vector<robosay::base::CSortManager<std::string>::NodeElem>::const_iterator iter = elems.begin();
		
		while (iter != elems.end())
		{
			std::cout << "weight	:" << iter->GetWeight() << "	elem:" << iter->GetElem() << std::endl;		
			vNewResult.push_back(iter->GetElem());
			++iter;
		}

		vResult.swap(vNewResult);
	}
}

int VKnowledgeBase::loadSemanticConf(const acl::FileName& sSemanticConf){
    acl::File file;
    bool bRet = file.read(sSemanticConf);
    if(!bRet){
        return -1;
    }

    const size_t uCnt = file.size();
    for(size_t uIdx = 0u; uCnt > uIdx; ++uIdx){
        acl::String& sLine = file[uIdx];

        StringVector vLines = sLine.separate(":");
        if(vLines.size() != 2){
			ELOGGER->print(true, FAILE_CALL_PARAM("VKnowledgeBase::loadSemanticConf", "[vLines.size() != 2]", sLine.c_str()));
            continue;
        }

        m_mSmtcPath.insert(std::make_pair(vLines[0], sSemanticConf.getPath() + "/" + vLines[1]));
    }

    return m_mSmtcPath.size();
}

std::string VKnowledgeBase::getKBDir(const acl::FileName& xml){
    if(m_sKBDir.empty()){
        return xml.getPath();
    }
    return m_sKBDir;
}

//返回值:bool true，使用当前知识库 false，不使用当前知识库
bool VKnowledgeBase::IsUseCurKB(robosay::base::KnowledgeBase* pCurKB)const
{
	if (NULL == pCurKB) { return false; }
	
	bool bIsCurKBExtendedKB = pCurKB->IsIvrKB();
	pCurKB->SetCode(m_strCode);

	//如果是扩展业务
	if (!m_strCode.empty())
	{
		return bIsCurKBExtendedKB;//当前知识库是扩展的就使用，否则不使用
	}
	
	//不是扩展业务
	return !bIsCurKBExtendedKB;//当前知识库不是扩展的就使用，否则不使用
}


}// namespace base
}// namespace robosay
