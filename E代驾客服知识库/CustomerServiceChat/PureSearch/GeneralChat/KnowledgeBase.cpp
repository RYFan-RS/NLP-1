

#include "CustomerServiceChat/PureSearch/GeneralChat/KnowledgeBase.h"
#include "SemanticAnalysis/RuleAnalysis/SmtcKeyWordMngr.h"

#include <Common/lib4/String.hpp>



#include <cstdio>
#include <iostream>

#include "Common/lib4/Utility.hpp"



#ifndef PRINT_LOG
#define PRINT_LOG
#endif
#include "Common/lib5/WHL_LogInstance.hpp"

#define TEMPLATE_QA_INFO_PART1  "<qa id =\"\" operator=\"0\">  \
<question id =\"\"><![CDATA["

#define TEMPLATE_QA_INFO_PART2  "]]></question>  \
<answer id =\"\" way =\"0\"><![CDATA[<answer> \
    <read_time>1000</read_time>               \
    <type>_CDATA_BEGtext_CDATA_END</type>     \
    <content>_CDATA_BEG"
	
#define TEMPLATE_QA_INFO_PART3	"_CDATA_END</content></answer>]]>  \
</answer>  \
</qa>"

#define TEMPLATE_QA_LAST_SENTENCE  "已经是最后一句了。"

float robosay::general::KnowledgeBase::m_gCosMinValue = 0.350;

namespace robosay{
namespace general{
KnowledgeBase::KnowledgeBase(const int nIdx)
	: robosay::base::KnowledgeBase("general", nIdx)
	, m_Entry()
	, m_vQA()
	,m_bDB(false){
}

KnowledgeBase::~KnowledgeBase(){

}
void KnowledgeBase::setCosMinValue(const std::string& configFileName){
    acl::File file;
	if(!file.read(configFileName)){
		std::cout<<"CosMinValue Now is "<<m_gCosMinValue<<std::endl;			
		return ;
	}

	std::cout<<std::endl;	

	for(size_t i=0; i<file.size(); i++) {
		if(file[i].empty()) continue;
		
		acl::String number = file[i];
		number.strip();

		if(acl::Utility::isNumber(number)){
			float value = number.to<float>();
			if(value>=0.001 && value<1){
				std::cout<<"CosMinValue "<<"previous is "<<m_gCosMinValue<<" Now is "<<value<<std::endl;
				m_gCosMinValue = value;
			}

		}

		break;
	}

	std::cout<<"CosMinValue Now is "<<m_gCosMinValue<<std::endl;	
}

bool KnowledgeBase::init(const acl::FileName& sXmlFile,std::string sCommonDicDir, const std::string& sSmtcPath){
	m_strKBFullFileName = sXmlFile;
	robosay::XmlParser xml;
	// 使用XML解析器解析XML文件
	if(xml.read(sXmlFile) == false) {
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::init", "xml.read", sXmlFile.c_str()));
		return false;
	}

	// 获取名为scml的根节点
	xmlNodePtr rootNode = xml.getRootNode("scml");
	if(rootNode == NULL){
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::init", "xml.getRootNode", sXmlFile.c_str()));
		return false;
	}

	bool bRet = readScmlAttribute(xml, rootNode);
	if(!bRet){
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::init","::readScmlAttribute", sXmlFile.c_str()));
		return false;
	}

	// 读取KB的相关属性
	if(m_Config.m_Attribute.init(sXmlFile,xml) == false){
        PRINT("Failed to init Attribute node'"<<sXmlFile<<"'.");
		return false;
	}
	
	//init ContextDictionary
	if (!InitContextDictionary(sXmlFile,sSmtcPath)) {
        return false;
	}


	#if  0	
	if(m_Config.init(sXmlFile, NULL, sCommonDicDir, sSmtcPath) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::init", "m_Config", sXmlFile.c_str()));
		return false;
	}


    if(!m_Config.m_bBasicSearch){
		ELOGGER->print(true, "in KnowledgeBase::init m_Config.m_bBasicSearch is false!\n");
        return false;
    }
	
	m_pMixParser = m_Config.m_basicSearch.getMixParserPointer();
	m_TFIDF.m_questionNum  = 0;	
	#endif	
	
	bRet = readKB(xml);
	if(!bRet){
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::init", "readKB", sXmlFile.c_str()));
		return false;
	}

	makeEntry();
	if(initPinyin(m_vQA, xml, sCommonDicDir) == false){
		ELOGGER->print(true, "KnowledgeBase::init [initPinyin(m_vQA, xml, sCommonDicDir) == false]: Failed to init PinYin.");
	}
	
	
	return true;
}

inline bool KnowledgeBase::equalWithLength(const std::string& s1, const std::string& s2, int nLen) const{
	for(size_t i=0; i<nLen; i++){
		if(s1[i] != s2[i]) return false;
	}
	
	return true;
}
inline void KnowledgeBase::replaceAllQuotations(std::string& s, size_t beg) const{
	s.insert(beg++,"\\");
	
	while(1){
		beg = s.find("\"",beg);
		if(beg == std::string::npos) break;
		s.insert(beg++,"\\");
	}
}

bool KnowledgeBase::getPromptInfo(const std::string& sIn, std::vector<std::string>& vResult, int count) const{
	int nLength = sIn.length();
			
	std::queue<int> qIdx;
	
	for(size_t i=0; i<m_vQA.size(); i++){
		const std::vector<Sentence>& vQuestion = m_vQA[i].getQuestions();
		for(size_t j=0; j<vQuestion.size(); j++){

			const std::string question = vQuestion[j].getSentence();

			if(nLength > question.length()) continue;
			
			if(equalWithLength(sIn,question,nLength) == true){
				qIdx.push(i);
				
				if(vResult.size() < count){
					size_t nPos = question.find("\"");
					if(nPos != std::string::npos){
						std::string sNewStr = question;
						replaceAllQuotations(sNewStr,nPos);
						vResult.push_back(sNewStr);
					}else{
						vResult.push_back(question);
					}
				}else{
					return true;
				}
			}
		}
		
		if(vResult.size() >= count) return true;
	}


	for(size_t i=0; i<m_vQA.size(); i++){
		const std::vector<Sentence>& vQuestion = m_vQA[i].getQuestions();

		if(qIdx.front() == i){
			qIdx.pop();
			continue;
		}
		
		for(size_t j=0; j<vQuestion.size(); j++){

			const std::string question = vQuestion[j].getSentence();

			if(nLength > question.length()) continue;
			
			if(question.find(sIn) != std::string::npos){
				if(vResult.size() < count){
					size_t nPos = question.find("\"");
					if(nPos != std::string::npos){
						std::string sNewStr = question;
						replaceAllQuotations(sNewStr,nPos);
						vResult.push_back(sNewStr);
					}else{
							vResult.push_back(question);
					}
				}else{
					return true;
				}
			}
		}	
		
		if(vResult.size() >= count) return true;
	}

	return true;
}

bool KnowledgeBase::initFromDB(acl::String fileName){

	m_bDB = true;
	m_kbInfo.set_id("0");
	m_kbInfo.set_type("FAQ");
	m_kbInfo.set_name("MySqlDB");
	m_kbInfo.set_industry("common");

	if(fileName.empty()){
		std::cout<<"QA file name is empty."<<std::endl;
		return false;
	}
	
	std::vector<acl::String> vecFiles = fileName.separate_(";");

	    // 第一步连接数据库  
	for(int i=0;i<vecFiles.size();i++){
		vecFiles[i].strip();

		acl::File contentFile;

		contentFile.read(vecFiles[i]);
		
		for(size_t j=0; j<contentFile.size(); j++){
			contentFile[j].toUpper();
			contentFile[j].strip();
			std::string::size_type loc = contentFile[j].find("\t");
			if(loc==std::string::npos || loc==0){
				continue;
			}

			std::string question = contentFile[j].substr(0,loc);
			std::string answer   = contentFile[j].substr(loc+1);

			if(question.empty() || answer.empty()){
				continue;
			}
			
			robosay::general::QA qa(m_vQA.size());
			if(qa.init(question,answer) == false) {
				ELOGGER->print(true, "KnowledgeBase::readKB [qa.init(xml,qaPtr) == false] Failed to init QA node.");
				continue;
			}
			
			m_vQA.push_back(qa);			
		}		
	}

	ELOGGER->print(true, "qa total number is [%d].\n", m_vQA.size());
	
	if(m_vQA.size()==0){
		return false;
	}
	return true;
}


bool KnowledgeBase::InitContextDictionary(const std::string& sXmlFile, const std::string& sSmtcPath)
{
	aisms::KBConfig kbConfig;
 	if(!kbConfig.init(sXmlFile.c_str())){ return false; }

	std::string strSemanticName = sSmtcPath + "/" + kbConfig.get("SemanticSearch", "semantic_dic");

	//if file is not exist, no handle
 	if (access(strSemanticName.c_str(), 0) != 0) { return true; }
	
	if(m_Config.m_se.init(strSemanticName, "", NULL) != 0)  
	{
		PRINT("Failed to init ContextDictionary : "<<strSemanticName);
		return false; 
	}	
	
	return true;
}

/*
bool KnowledgeBase::initTfidf(aisms::mxpsr::Parser *parser, TFIDF* pTFIDF){
	if(initTfidf(m_vQA) == false){
		return false;
	}


	for(size_t i=0; i<vQA.size(); i++){
		std::vector<Sentence>& vQuestion = vQA[i].getQuestionsRefer();
		for(size_t j=0; j<vQuestion.size(); j++){
			//const StringVector& vWord = vQuestion[j].getVWord();

			std::string sSentence = vQuestion[j].getSentence();
			acl::String s(sSentence);
			const std::vector<std::string>& vWord = s.toCharactorWithLatin();
			
			// 将输入字符串中包含的中文字符转换成拼音表示
			const StringVector vWordPinyin = m_PINYIN.Hanzi2Pinyin(vWord);
			vQuestion[j].setPinyin(vWordPinyin);
		}
	}

	return true;
}
*/

// 分词引擎
StringVector KnowledgeBase::parse(const std::string &sInput) {
	// 获取由MixParser的dynamic解析出来的含标签的Words
	Words ws;
	if(!aisms::gOption.sDate.empty()){
		ws = m_Config.m_Parser.dynamicParse(sInput, aisms::gOption.sDate);
	}else{
		ws = m_Config.m_Parser.dynamicParse(sInput);
	}
	return ws.getWords();
}

// 初始化 TFIDF 搜索
bool KnowledgeBase::initIDF(robosay::KeyWordSearch* pKeyWordSearch){
	std::map<std::string,int>::const_iterator iter;
	std::vector<TfidfValue> vTfidfValue;
	
	//m_parser = parser;
	//m_pTFIDF = pTFIDF;
	m_pKeyWordSearch = pKeyWordSearch;

	std::map<std::string,std::vector<std::string> >subSentence;
	/*计算question的IDF*/
	for(size_t i=0; i<m_vQA.size(); i++){
		const std::vector<Sentence>& vQuestion = m_vQA[i].getQuestions();
		//PRINT(vQuestion.size())
		for(size_t j=0; j<vQuestion.size(); j++){
			//PRINT(vQuestion.size())
			std::set<int> appeared;
			//int appeared = j;

			/*只进行切词,不会进行日期或者噪音词典的过滤*/
			std::vector<std::string> vGetWord = m_Config.m_Parser.parseWithJieba(vQuestion[j].getSentence(),
			                                               subSentence).getWords();
			const_cast< std::vector<Sentence>& >(vQuestion).at(j).setWords(vGetWord);
			/*
			m_pKeyWordSearch->filterStopWord(vGetWord);
			if(vGetWord.size() == 0){
				continue;
			}
			*/	
			m_pKeyWordSearch->m_TFIDF.m_questionNum += 1;			
			//const_cast< std::vector<Sentence>& >(vQuestion).at(j).setWords(vGetWord);
			//vQuestion[j].setWords(vGetWord);
			std::vector<std::vector<std::string> > allSegmentMethod = m_pKeyWordSearch->getAllSegmentMethod(vGetWord,subSentence);
			
			//const StringVector& vWord = vQuestion[j].getVWord();
			
			std::vector<std::vector<std::string> >::iterator itr = allSegmentMethod.begin();
			for(;itr!=allSegmentMethod.end();itr++){
				std::vector<int>  wordsId;
				StringVector& vWord = *itr;
				m_pKeyWordSearch->filterStopWord(vWord);
				for(size_t k=0; k<vWord.size(); k++){
					iter = m_pKeyWordSearch->m_TFIDF.m_word_idx.find(vWord[k]);
					if(iter == m_pKeyWordSearch->m_TFIDF.m_word_idx.end()){
						
						appeared.insert(m_pKeyWordSearch->m_TFIDF.m_word_idx.size());
						wordsId.push_back(m_pKeyWordSearch->m_TFIDF.m_word_idx.size());
						m_pKeyWordSearch->m_TFIDF.m_word_idx.insert(std::make_pair(vWord[k],m_pKeyWordSearch->m_TFIDF.m_word_idx.size()));
						m_pKeyWordSearch->m_TFIDF.m_word_frequency.insert(std::make_pair(m_pKeyWordSearch->m_TFIDF.m_word_idx.size()-1,1));
						//appeared += 1;
					}else{
						wordsId.push_back(iter->second);
						if(appeared.find(iter->second) == appeared.end()){
							m_pKeyWordSearch->m_TFIDF.m_word_frequency[iter->second]++;   //出现该分词的总question数目
							appeared.insert(iter->second);					
						}
					}
				}
				if(wordsId.size()!=0){
					const_cast< std::vector<Sentence>& >(vQuestion).at(j).addOneSegment(wordsId);
				}
			}
		}
	}

	/*计算关键字的IDF
	for(size_t i=0; i<m_vQA.size(); i++){
		//PRINT(vQuestion.size())
		#ifdef TAG_KEYWORD_IS_ONE_DOCU
		const std::vector< std::vector<std::string> >&vDocument = m_vQA[i].getDicKeyWords();

		m_pKeyWordSearch->m_TFIDF.m_questionNum += vDocument.size();
		for(size_t j=0; j<vDocument.size(); j++){
			//PRINT(vQuestion.size())

			std::set<int> appeared;
			//int appeared = j;

			const StringVector& vWord = vDocument[j];
			
			for(size_t k=0; k<vWord.size(); k++){
				iter = m_pKeyWordSearch->m_TFIDF.m_word_idx.find(vWord[k]);
				if(iter == m_pKeyWordSearch->m_TFIDF.m_word_idx.end()){
					
					appeared.insert(m_pKeyWordSearch->m_TFIDF.m_word_idx.size());
					m_pKeyWordSearch->m_TFIDF.m_word_idx.insert(std::make_pair(vWord[k],m_pKeyWordSearch->m_TFIDF.m_word_idx.size()));
					m_pKeyWordSearch->m_TFIDF.m_word_frequency.insert(std::make_pair(m_pKeyWordSearch->m_TFIDF.m_word_idx.size()-1,1));

					//appeared += 1;
				}else{
					if(appeared.find(iter->second) == appeared.end()){
						m_pKeyWordSearch->m_TFIDF.m_word_frequency[iter->second]++;   //出现该分词的总question数目
						appeared.insert(iter->second);					
					}
				}
			}
		}				
		#else
		const std::vector<Sentence>& vQuestion = m_vQA[i].getKeyWordsRefer();		
		m_pKeyWordSearch->m_TFIDF.m_questionNum += vQuestion.size();
		for(size_t j=0; j<vQuestion.size(); j++){
			//PRINT(vQuestion.size())

			std::set<int> appeared;
			//int appeared = j;

			const StringVector& vWord = vQuestion[j].getVWord();
			for(size_t k=0; k<vWord.size(); k++){
				iter = m_pKeyWordSearch->m_TFIDF.m_word_idx.find(vWord[k]);
				if(iter == m_pKeyWordSearch->m_TFIDF.m_word_idx.end()){
					
					appeared.insert(m_pKeyWordSearch->m_TFIDF.m_word_idx.size());
					m_pKeyWordSearch->m_TFIDF.m_word_idx.insert(std::make_pair(vWord[k],m_pKeyWordSearch->m_TFIDF.m_word_idx.size()));
					m_pKeyWordSearch->m_TFIDF.m_word_frequency.insert(std::make_pair(m_pKeyWordSearch->m_TFIDF.m_word_idx.size()-1,1));

					//appeared += 1;
				}else{
					if(appeared.find(iter->second) == appeared.end()){
						m_pKeyWordSearch->m_TFIDF.m_word_frequency[iter->second]++;   //出现该分词的总question数目
						appeared.insert(iter->second);					
					}
				}
			}
		}		
		//dasdada
		#endif
	}
	*/
	return true;
}
bool KnowledgeBase::calculateTfidf(){
	// 计算所有录入问题的 TFIDF 形式
	std::vector<TfidfValue> vTfidfValue;
	std::vector<std::string> sOutput;
	
	for(size_t i=0; i<m_vQA.size(); i++){
		std::vector<Sentence>& vQuestion = m_vQA[i].getQuestionsRefer();
		for(size_t j=0; j<vQuestion.size(); j++){
			const std::vector<std::vector<int> >& segmentMethods = vQuestion[j].getAllSegmentMethods(); 
			//const StringVector& vWord = vQuestion[j].getVWord();
			// 转换字符串单词到数字单词
			std::vector<std::vector<int> >::const_iterator itr = segmentMethods.begin();
			for(;itr != segmentMethods.end();itr++){
				//const std::vector<int> vWordIdx = m_pKeyWordSearch->m_TFIDF.transWords(vWord,sOutput);
				vTfidfValue = m_pKeyWordSearch->m_TFIDF.caculate(*itr);
				vQuestion[j].addTfidfValue(vTfidfValue);
			}
		}

		/*
		sOutput.clear();
		std::vector<Sentence>& vKeyWord = m_vQA[i].getKeyWordsRefer();
		for(size_t j=0; j<vKeyWord.size(); j++){
			const StringVector& vWord = vKeyWord[j].getVWord();
			// 转换字符串单词到数字单词
			const std::vector<int> vWordIdx = m_pKeyWordSearch->m_TFIDF.transWords(vWord,sOutput);
			vTfidfValue = m_pKeyWordSearch->m_TFIDF.caculate(vWordIdx);
			vKeyWord[j].setTfidfValue(vTfidfValue);
		}
		*/
	}

	return true;
}

// 初始化 PINYIN 搜索
bool KnowledgeBase::initPinyin(std::vector<QA>& vQA,robosay::XmlParser& xml, std::string sCommonDicDir){
	// 读入 HANZI-PINYIN的字典
	acl::File Hanzi2PinyinDic;
	std::string line, Hanzi, Pinyin;
	std::string sPinyinDic = xml.getXPathValue("//scml/config/BasicSearch/pinyin_dic");
	if(sPinyinDic.empty()){
		std::cerr<<"Warning: Failed to get\"//scml/config/BasicSearch/pinyin_dic\" node!"<<std::endl;
		return false;
	}

    if(sCommonDicDir.empty()){
        sCommonDicDir = m_Config.m_Attribute.getFilePath();
    }else{
        sCommonDicDir = sCommonDicDir + "/" +  m_Config.m_Attribute.getKBIndustry();
    }

    acl::FileName sDicPath = sCommonDicDir + "/" + sPinyinDic;
#ifdef USE_ENCODE_DECODE
    StringVector vWordsDic;
    acl::FileEncoder encoder;
    std::string sFileFullPath = sDicPath.getBaseName()+".dat";
    encoder.decodeFile(sFileFullPath, vWordsDic);
	for(size_t j=0; j<vWordsDic.size(); j++){
		std::string line = vWordsDic[j];
#else
	Hanzi2PinyinDic.read(sDicPath);
	for(size_t j=0; j<Hanzi2PinyinDic.size(); j++){
        std::string line = Hanzi2PinyinDic[j];
#endif
		int HanziBeg = 0, HanziEnd;
		int PinyinBeg, PinyinEnd;
		int lineEnd = line.length();

		// 查找 Hanzi 的 结束End 位置
		int tmp = 0;
		while(tmp < lineEnd && (line[tmp] != '\t' && line[tmp] != ' ')){
			tmp++;
		}
		HanziEnd = tmp;

		// 查找 Pinyin 的 起始Beg 和 结束End 位置
		while(tmp < lineEnd && (line[tmp] == '\t' || line[tmp] == ' ')){
			tmp++;
		}
		PinyinBeg = tmp ;
		tmp = lineEnd-1;
		while(tmp > -1 && (line[tmp] == '\t' || line[tmp] == ' ' || line[tmp] == '\r' || line[tmp] == '\n')){
			tmp--;
		}
		PinyinEnd = tmp + 1;

		Hanzi = line.substr(HanziBeg, HanziEnd - HanziBeg);
		Pinyin = line.substr(PinyinBeg, PinyinEnd - PinyinBeg);
		m_PINYIN.m_hanzi_pinyin.insert(std::make_pair(Hanzi, Pinyin));
	}


	for(size_t i=0; i<vQA.size(); i++){
		std::vector<Sentence>& vQuestion = vQA[i].getQuestionsRefer();
		for(size_t j=0; j<vQuestion.size(); j++){
			acl::String s(vQuestion[j].getSentence());
			StringVector vIn = s.toCharactorWithLatin();
			// 将输入字符串中包含的中文字符转换成拼音表示
			const StringVector vWordPinyin = m_PINYIN.Hanzi2Pinyin(vIn);
			vQuestion[j].setPinyin(vWordPinyin);
		}
	}

	return true;
}


// 基于 Pinyin 的匹配
bool KnowledgeBase::searchByPinyin(const std::string& sIn, robosay::base::VResult& vResult, const std::string& sWay){
	float tmpScore = 0.0;
	int tmpIndex = -1, questionIdx = -1;
	StringVector vWordPinyin;
	robosay::base::Result* pRst = createResult();
	scoped_ptr<robosay::base::Result> autoPtrObj(pRst);

	// 输入转拼音
	acl::String s(sIn);
	StringVector vIn = s.toCharactorWithLatin();
	vWordPinyin = m_PINYIN.Hanzi2Pinyin(vIn);

	for(size_t i=0; i<m_vQA.size(); i++){
		float fScore = m_vQA[i].matchByPinyin(vWordPinyin, questionIdx);
		if(fScore > 0.75){
			if(fScore > tmpScore){
				tmpScore = fScore;
				tmpIndex = i;
			}
		}
	}
	// 找到最优的答案
	if(tmpIndex != -1){
		pRst->m_pPath->setPath(&m_vQA[tmpIndex]);
		pRst->setScore(tmpScore);
        StringStringMap mEmpty;
        pRst->setRelateQuestions(getRelateQuestion(tmpIndex, sIn));
        pRst->setAnswerInfo(m_vQA[tmpIndex].getAnswerInfo(mEmpty, NULL, sWay));
        const Sentence& stc = m_vQA[tmpIndex].getQuestion(questionIdx);
        pRst->setTarget(stc.getSentence(), stc.getID());
		pRst->setQuestionType(Question_info::SearchByPinyinAnwser);				
        autoPtrObj.detach();
        vResult.push_back(pRst);
	}

	return true;
}

// 基于 TFIDF 的匹配
bool KnowledgeBase::searchByTFIDF(const std::string& sIn, robosay::base::VResult& vResult, const std::string& sWay,  
									const std::vector<std::vector<TfidfValue> >& vInTfidf){

	int questionIdx = -1;
	
	robosay::base::CSortManager<int,float>  sortManager;
	std::map<int,int>  mapIndexQuestion;


	// 分词处理
	//StringVector vWord = parse(sIn);

	// 标记forcehit


	// 转换字符串单词到数字单词
	//std::vector<int> vWordIdx = m_pKeyWordSearch->m_TFIDF.transWords(vWord);

	// 匹配
	for(size_t i=0; i<m_vQA.size(); i++){
		float fScore = m_vQA[i].matchByTfidf(vInTfidf, questionIdx,m_pKeyWordSearch->m_TFIDF.m_tfIdfRemend);
		//std::cout << "======> fScore:" << fScore << std::endl;
		if(fScore > m_gCosMinValue){ //0.680   0.35
			sortManager.Add(fScore,i);
			mapIndexQuestion.insert(std::make_pair(i,questionIdx));
		}
	}

	sortManager.Sort();
	
	const std::vector< robosay::base::CSortManager<int,float>::NodeElem >& elems = sortManager.GetAllElems();
	
	std::vector<robosay::base::CSortManager<int,float>::NodeElem>::const_iterator iter = elems.begin();
	
	for(int i=0;iter!=elems.end();iter++,i++){
		std::map<int,int>::const_iterator itr = mapIndexQuestion.find(iter->GetElem());
		if(itr!=mapIndexQuestion.end()){
			robosay::base::Result* pRst = createResult();
			scoped_ptr<robosay::base::Result> autoPtrObj(pRst);	
			if(pRst==NULL)
				return false;
			pRst->m_pPath->setPath(&m_vQA[iter->GetElem()]);
			pRst->setScore(iter->GetWeight());
	        StringStringMap mEmpty;
	        pRst->setRelateQuestions(getRelateQuestion(iter->GetElem(), sIn));
	        pRst->setAnswerInfo(m_vQA[iter->GetElem()].getAnswerInfo(mEmpty, NULL, sWay));
	        const Sentence& stc = m_vQA[iter->GetElem()].getQuestion(itr->second);
			if(stc.getSentence().empty()){
				const std::vector<std::string>& vWord = stc.getVWord();
				std::string result;
				std::vector<std::string>::const_iterator it = vWord.begin();
				std::cout << "匹配到的结果：";
				for(;it!=vWord.end();it++){
					result+=*it;
					result+=",";
					std::cout << *it << "/";
				}
				std::cout << std::endl;
	    		pRst->setTarget(result,"");			
			}
			else{
	    		pRst->setTarget(stc.getSentence(), stc.getID());
			}
			pRst->setQuestionType(Question_info::KeyWordAnwser);		
	        autoPtrObj.detach();
	        vResult.push_back(pRst);			
		}
	}

	/*
	if(tmpIndex != -1){
		pRst->m_pPath->setPath(&m_vQA[tmpIndex]);
		pRst->setScore(tmpScore);
        StringStringMap mEmpty;
        pRst->setRelateQuestions(getRelateQuestion(tmpIndex, sIn));
        pRst->setAnswerInfo(m_vQA[tmpIndex].getAnswerInfo(mEmpty, NULL, sWay));
        const Sentence& stc = m_vQA[tmpIndex].getQuestion(questionRet);
		if(stc.getSentence().empty()){
			const std::vector<std::string>& vWord = stc.getVWord();
			std::string result;
			std::vector<std::string>::const_iterator it = vWord.begin();
			std::cout << "匹配到的结果：";
			for(;it!=vWord.end();it++){
				result+=*it;
				result+=",";
				std::cout << *it << "/";
			}
			std::cout << std::endl;
    		pRst->setTarget(result,"");			
		}
		else{
    		pRst->setTarget(stc.getSentence(), stc.getID());
		}
		pRst->setQuestionType(Question_info::KeyWordAnwser);		
        autoPtrObj.detach();
        vResult.push_back(pRst);
	}
	*/
	return true;
}


// 基于改进的 Levenshtein Distance 的匹配
bool KnowledgeBase::searchByLevenshteinDistance(const std::string& sIn, robosay::base::VResult& vResult, const std::string& sWay,
									const robosay::base::VResult* pMatchResult){
	int questionIdx = -1;

	robosay::base::CSortManager<int,float>  sortManager;
	std::map<int,int>  mapIndexQuestion;

	// 匹配
	// 改进的 Levenshtein 匹配
	// stringA 长度为 aLen 和 stringB 长度为 bLen, 两者的重复的字符串的长度总的长度为 sameLen
	// 计算方式 (sameLen * 2) / (aLen + bLen)
	// 当上个式子的取值大于0.70，认为两个问题相似，0.70是一个很大的相似度关系
	for(size_t i=0; i<m_vQA.size(); i++){
		float fScore = m_vQA[i].matchByLD(sIn, questionIdx);
		if(fScore > 0.75){
			sortManager.Add(fScore,i);
			mapIndexQuestion.insert(std::make_pair(i,questionIdx));
		}
	}

	sortManager.Sort();
	
	const std::vector< robosay::base::CSortManager<int,float>::NodeElem >& elems = sortManager.GetAllElems();
	
	std::vector<robosay::base::CSortManager<int,float>::NodeElem>::const_iterator iter = elems.begin();
	
	for(int i=0;iter!=elems.end();iter++,i++){
		std::map<int,int>::const_iterator itr = mapIndexQuestion.find(iter->GetElem());
		if(itr!=mapIndexQuestion.end()){
			robosay::base::Result* pRst = createResult();
			scoped_ptr<robosay::base::Result> autoPtrObj(pRst);	
			if(pRst==NULL)
				return false;
			pRst->m_pPath->setPath(&m_vQA[iter->GetElem()]);

			pRst->setScore(iter->GetWeight());
	        StringStringMap mEmpty;
	        pRst->setRelateQuestions(getRelateQuestion(iter->GetElem(), sIn));
	        pRst->setAnswerInfo(m_vQA[iter->GetElem()].getAnswerInfo(mEmpty, NULL, sWay));
	        const Sentence& stc = m_vQA[iter->GetElem()].getQuestion(itr->second);

			if(pMatchResult!=NULL && pMatchResult->PathInVResult(pRst)){
				std::cout<<"Levenshtein result is in the MatchResult.skip it."<<stc.getSentence()<<std::endl;
				continue;
			}
			
			if(stc.getSentence().empty()){
				const std::vector<std::string>& vWord = stc.getVWord();
				std::string result;
				std::vector<std::string>::const_iterator it = vWord.begin();
				std::cout << "匹配到的结果：";
				for(;it!=vWord.end();it++){
					result+=*it;
					result+=",";
					std::cout << *it << "/";
				}
				std::cout << std::endl;
	    		pRst->setTarget(result,"");			
			}
			else{
	    		pRst->setTarget(stc.getSentence(), stc.getID());
			}
			pRst->setQuestionType(Question_info::SearchByLevenshteinDistanceAnwser);		
	        autoPtrObj.detach();
	        vResult.push_back(pRst);			
		}
	}
	
	return true;
}

bool KnowledgeBase::readKB(robosay::XmlParser& xml){

	//Initialize Semantic KeyWordManager
	sae::CSmtcKeyWordMngr smticKeyWordMngr;
	smticKeyWordMngr.MatchSmtcAndKeyWord(m_Config.m_se.getData().GetConstKeyWordManager(),m_Config.m_se.getData().getConstTagManager());
	
	// 获取名为scml的根节点
	xmlNodePtr rootNode = xml.getRootNode("scml");
	if(rootNode == NULL){
		return false;
	}
	xmlNodePtr qaPtr = xml.getFirstChildNode(rootNode,"qa");
	while(qaPtr != NULL) {
		// 若不是元素节点则看下一个
		if(qaPtr->type != XML_ELEMENT_NODE){
			qaPtr = qaPtr->next;
			continue;
		}

		robosay::general::QA qa(m_vQA.size());
		if(qa.init(xml,qaPtr) == false) {
			ELOGGER->print(true, "KnowledgeBase::readKB [qa.init(xml,qaPtr) == false] Failed to init QA node.");
			return false;
		}
		//set qa keyword
		smticKeyWordMngr.ForEachBySemantic(qa.getCurSemantic(),bind(&robosay::general::QA::SetKeyWord,&qa,_1));
		m_vQA.push_back(qa);
		qaPtr = qaPtr->next;
	}

	analyseTangPoetry();
	return true;
}

void KnowledgeBase::OutputInFile(CTestJuiWei& testJuiWei)
{
	robosay::XmlParser xml;
	// 使用XML解析器解析XML文件
	if(xml.read(m_strKBFullFileName) == false) {
		ELOGGER->print(true, FAILE_CALL_PARAM("KnowledgeBase::OutputInFile", "xml.read", m_strKBFullFileName.c_str()));
		return ;
	}

	// 获取名为scml的根节点
	xmlNodePtr rootNode = xml.getRootNode("scml");
	if(rootNode == NULL) { return ; }
	
	xmlNodePtr qaPtr = xml.getFirstChildNode(rootNode,"qa");

	for (; qaPtr!=NULL; qaPtr=qaPtr->next) 
	{
		// 若不是元素节点则看下一个
		if(qaPtr->type != XML_ELEMENT_NODE) { continue; }

		xmlNodePtr ptr = xml.getFirstChildNode(qaPtr);

		std::string strSemantic, strAnswer;
		
		for (; ptr!=NULL; ptr=ptr->next) 
		{
			// 若不是元素节点则看下一个
	  		if(ptr->type != XML_ELEMENT_NODE){ continue; }

			if(!xmlStrcmp(ptr->name, BAD_CAST "semantic")) 
			{
				strSemantic = xml.getValue(ptr);	
			}
			else if(!xmlStrcmp(ptr->name, BAD_CAST "answer")) 
			{
				strAnswer = xml.getValue(ptr);		
			}	
		}
	
		testJuiWei.Output(strSemantic,strAnswer);
	}	
}

#if 0
void KnowledgeBase::getKeyWordsDictionary(std::set<std::string>& dicKeyWords){
	std::vector<robosay::general::QA>::const_iterator it = m_vQA.begin();
	for(;it!=m_vQA.end();it++){
		const std::vector< std::vector<std::string> >& dic = it->getDicKeyWords();

		std::vector< std::vector<std::string> >::const_iterator itWord = dic.begin();
		for(;itWord!=dic.end();itWord++){
			const std::vector<std::string> &words = *itWord;
			
			std::vector<std::string>::const_iterator itrEle = words.begin();
			for(;itrEle!=words.end();itrEle++){
				dicKeyWords.insert(*itrEle);
			}
		}
	}
}
#endif

bool KnowledgeBase::analyseTangPoetry(){
	int index=0,size = m_vQA.size();
	xmlNodePtr qaPtr = NULL;
		
	if(m_kbInfo.get_level() == "3"){//生成新的虚拟QA，来完成Answer的顺序问答索引
		std::vector<robosay::general::QA>::const_iterator itr = m_vQA.begin();
		std::vector<robosay::general::QA> vNewQAs;
		for(; itr != m_vQA.end(); ++itr){
			std::string sQuestion = itr->getQuestion(0).getSentence();

			if(itr->m_vWebAnswer.size() < 1)
			{
				std::cout<< "warnning:sQuestion is "<<sQuestion<<" answer is empty."<<std::endl;
				continue;
			}
			
			std::string  sAnswerContent =  itr->m_vWebAnswer[0].getAnswerInfo(StringStringMap(),NULL).getAnswerContent();
			std::vector<std::string> vecReturn;
			
			std::cout<<"sQuestion is "<<sQuestion<<" answer is "<<sAnswerContent<<std::endl;	
			
			cutSentenceFromAnswer(sAnswerContent,vecReturn);
			
			for(int i=0;i<vecReturn.size();i++,index++)
			{
				std::string newQAStr;
				if(i==vecReturn.size()-1){
					newQAStr = TEMPLATE_QA_INFO_PART1 + vecReturn[i] + 
								+TEMPLATE_QA_INFO_PART2 + TEMPLATE_QA_LAST_SENTENCE
								+sQuestion+":"+sAnswerContent+TEMPLATE_QA_INFO_PART3;
				}
				else
				{
					newQAStr = TEMPLATE_QA_INFO_PART1 + vecReturn[i]
								+TEMPLATE_QA_INFO_PART2+vecReturn[i+1]+TEMPLATE_QA_INFO_PART3;
				}
				robosay::XmlParser newXml;
				if(false == newXml.parse(newQAStr))
				{
					std::cout<< "error:newXml.parse(newQAStr)"<<std::endl;
					return false;
				}
				qaPtr = newXml.getRootNode("qa");
				if(qaPtr == NULL){
					std::cout<< "error:qaPtr == NULL."<<std::endl;
					return false;
				}
				robosay::general::QA qa(size+index);
				if(qa.init(newXml,qaPtr) == false) {
					std::cout<< "qa.init(newXml,qaPtr) == false."<<std::endl;
					return false;
				}
				vNewQAs.push_back(qa);
			}
		}
		
		itr = vNewQAs.begin();
		for(;itr != vNewQAs.end();itr++)
		{
			m_vQA.push_back(*itr);
		}
	}

	return true;
}

void KnowledgeBase::cutSentenceFromAnswer(const acl::String answer,std::vector<std::string>& vecReturn)
{
		std::set<std::string> sFindStr;
		sFindStr.insert(",");
		sFindStr.insert(".");
		sFindStr.insert("?");
		sFindStr.insert("!");
		sFindStr.insert("，");
		sFindStr.insert("。");
		sFindStr.insert("？");
		sFindStr.insert("！");
		sFindStr.insert("；");
		sFindStr.insert(":");
		sFindStr.insert(";");	

		answer.cutSentenceFromAnswer(sFindStr,vecReturn);
}

void KnowledgeBase::makeEntry() {
	const std::vector<robosay::general::QA>::const_iterator last = m_vQA.end();
	std::vector<robosay::general::QA>::const_iterator itr = m_vQA.begin();
	StringIntSetMap& mWordIdxs = m_Entry.getWordIdxs();
	for(; last != itr; ++itr){
		const StringStringMap::const_iterator l = itr->m_mSemantics.end();
		StringStringMap::const_iterator i = itr->m_mSemantics.begin();
		for(; l != i; ++i){
			StringIntSetMap::iterator itrFind = mWordIdxs.find(i->first);
			if(itrFind == mWordIdxs.end()){
				IntSet se;
				se.insert(itr->m_nIdx);
				mWordIdxs.insert(std::make_pair(i->first, se));
			}else{
				itrFind->second.insert(itr->m_nIdx);
			}
		}
	}
}

bool KnowledgeBase::execute(const std::string& sText, bool bNeedAnalysis, robosay::base::VResult& vResult, const std::string& sWay, const robosay::base::VResult* pPre /*= NULL*/){
	if(!bNeedAnalysis){
		return false;
	}
	robosay::base::Input* input = createInput();
	scoped_ptr<robosay::base::Input> autoPtrObj(input);
	input->m_sIn = sText;
	chat(*input,vResult, sWay);
	if(vResult.empty()){
		vResult.sMessage = "[ERROR]:未找到对应的answer";
		return false;
	}

	return true;
}

bool KnowledgeBase::execute(const std::string& sText, bool bNeedAnalysis, robosay::base::VResult& vResult, const std::string& sWay, 
							const std::vector<std::vector<TfidfValue> >& vInTfidf){
	if(!bNeedAnalysis){
		return false;
	}
	robosay::base::Input* input = createInput();
	scoped_ptr<robosay::base::Input> autoPtrObj(input);
	input->m_sIn = sText;
	chat(*input,vResult, sWay,vInTfidf);
	if(vResult.empty()){
		vResult.sMessage = "[ERROR]:未找到对应的answer";
		return false;
	}

	return true;
}

bool KnowledgeBase::chat(robosay::base::Input& input,robosay::base::VResult& vResult, const std::string& sWay,
						  const std::vector<std::vector<TfidfValue> >& vInTfidf){
	robosay::base::VResult vMidRslt;

	// 采用TFIDF 计算问题的匹配程度, 银联去掉词频匹配
	searchByTFIDF(input.m_sIn, vMidRslt, sWay,vInTfidf);
	if(!vMidRslt.empty()){
		vResult.moveFrom(vMidRslt);
		//ELOGGER->print(true, "KnowledgeBase::chat [!vMidRslt.empty()]");
		//return true;
	}

	// 采用改进的 Levenshtein Distance 计算问题的匹配程度
	searchByLevenshteinDistance(input.m_sIn, vMidRslt, sWay,&vResult);
	if(!vMidRslt.empty()){
			vResult.moveFrom(vMidRslt);
	}

	#if 0
	// 汉字转拼音，然后再进行基于拼音的匹配
	searchByPinyin(input.m_sIn, vMidRslt, sWay);
	if(!vMidRslt.empty()){
		if(!vResult.PathInVResult(vMidRslt[0]))
		{
			vResult.moveFrom(vMidRslt);
		}
		//ELOGGER->print(true, "KnowledgeBase::chat [!vMidRslt.empty()]");
		//return true;
	}
	#endif
	
	vResult.sort();

	insertKBIdx(vResult);		
	ELOGGER->print(true, "");
	for(size_t uIdx = 0u; uIdx < vResult.size(); ++uIdx){
		std::string sAnswer = vResult[uIdx]->getAnswer();
		ELOGGER->print(true, "BasicMatch Score[%d]\n", vResult[uIdx]->getScore());
		ELOGGER->print(true, "BasicMatch Aanswer[%s]\n", sAnswer.c_str());
		ELOGGER->print(true, "BasicMatch question[%s]type[%s]\n", vResult[uIdx]->getTarget().c_str(),
								vResult[uIdx]->getQuestionInfo().get_type().c_str());	
	}

	return true;
}

bool KnowledgeBase::chat(robosay::base::Input& input,robosay::base::VResult& vResult, const std::string& sWay){
	robosay::base::VResult vMidRslt;

	// 采用改进的 Levenshtein Distance 计算问题的匹配程度
	searchByLevenshteinDistance(input.m_sIn, vMidRslt, sWay);
	if(!vMidRslt.empty()){
		vResult.moveFrom(vMidRslt);
		//ELOGGER->print(true, "KnowledgeBase::chat [!vMidRslt.empty()]");
		//return true;
	}

	// 汉字转拼音，然后再进行基于拼音的匹配
	searchByPinyin(input.m_sIn, vMidRslt, sWay);
	if(!vMidRslt.empty()){
		vResult.moveFrom(vMidRslt);
		//ELOGGER->print(true, "KnowledgeBase::chat [!vMidRslt.empty()]");
		//return true;
	}

	// 采用TFIDF 计算问题的匹配程度, 银联去掉词频匹配
	#if 0
	searchByTFIDF(input.m_sIn, vMidRslt, sWay);
	if(!vMidRslt.empty()){
		vResult.moveFrom(vMidRslt);
		//ELOGGER->print(true, "KnowledgeBase::chat [!vMidRslt.empty()]");
		//return true;
	}
	#endif
	
	insertKBIdx(vResult);	
	ELOGGER->print(true, "");
	for(size_t uIdx = 0u; uIdx < vResult.size(); ++uIdx){
		std::string sAnswer = vResult[uIdx]->getAnswer();
		ELOGGER->print(true, "BasicMatch Score[%d]\n", vResult[uIdx]->getScore());
		ELOGGER->print(true, "BasicMatch Aanswer[%s]\n", sAnswer.c_str());
		
	}

	return true;
}

std::vector<std::string> KnowledgeBase::getRelateQuestion(const int cur_idx, const std::string& sText){
        StringVector vRets;
        if(m_kbInfo.get_level() != "2"){
            return vRets;
        }

    	const size_t uCnt = m_vQA.size();
    	for(size_t uIdx = 0u; uIdx < uCnt; ++uIdx){
            if(uIdx != cur_idx){
				const std::string&  result = m_vQA[uIdx].getRelateQuestion();
				if(!result.empty()){
			    	vRets.push_back(result);	
					continue;
				}
				
                const std::vector<Sentence>& sentences = m_vQA[uIdx].getQuestions();
                const std::vector<Sentence>::const_iterator last = sentences.end();
                std::vector<Sentence>::const_iterator itr = sentences.begin();
                for(; last != itr; ++itr){
                    vRets.push_back(itr->getSentence());
                }
            }
    	}
    	return vRets;
}

void KnowledgeBase::GetRandomRelateQuestion(std::vector<std::string>& randomRelateQuestion, int randomNum)const
{
	if(randomNum < 0) { return; }
	int iTotalNum = m_vQA.size();
	if (randomNum > iTotalNum) { randomNum = iTotalNum; }

	std::set<int> indexRets;
	std::set<int> indexOfEmptyQues;
	srand(time(NULL));	

	int indexNum = 0;
	while (indexNum < randomNum)
	{
		//防止死循环
		if ((indexRets.size()+indexOfEmptyQues.size()) >= iTotalNum){ break; }
		
		int index = rand() % iTotalNum;
		if (indexRets.find(index) != indexRets.end()){ continue; }

		std::string strQuestion = m_vQA[index].getRelateQuestion();
		if (strQuestion.empty()) 
		{
			indexOfEmptyQues.insert(index);
			continue; 
		}
		
		indexRets.insert(index);
		randomRelateQuestion.push_back(strQuestion);
		
		indexNum++;
	}
}


}//namespace general
}//namespace robosay

