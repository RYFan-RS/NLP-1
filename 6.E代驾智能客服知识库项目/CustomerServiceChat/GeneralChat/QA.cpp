#include "QA.h"
#ifndef PRINT_LOG
#define PRINT_LOG
#endif
#include "Common/lib5/WHL_LogInstance.hpp"
#include "CustomerServiceChat/Common/KeyWordAnalyse.h"

namespace robosay{
namespace general{

bool QA::init(const robosay::XmlParser& xml, xmlNodePtr qaPtr){
	xmlNodePtr ptr = xml.getFirstChildNode(qaPtr);
	robosay::general::Sentence sentence;
	std::string semanticStr;
	
	while(ptr != NULL) {
		// 若不是元素节点则看下一个
		if(ptr->type != XML_ELEMENT_NODE){
			ptr = ptr->next;
			continue;
		}

		if(!xmlStrcmp(ptr->name, BAD_CAST "semantic")) {
			std::string sID = xml.getAttribute(ptr, "id");
			semanticStr = xml.getValue(ptr);
			m_mSemantics.insert(std::make_pair(xml.getValue(ptr), sID));
		} else if(!xmlStrcmp(ptr->name, BAD_CAST "question")) {
			acl::String sQuestion = xml.getValue(ptr);		
			sQuestion.toUpper();
			sQuestion.removeChars("\t\r\n");			
			std::string sID = xml.getAttribute(ptr, "id");
			sentence.setSentence(sQuestion, sID);
			m_vQuestion.push_back(sentence);
		}else if(!xmlStrcmp(ptr->name, BAD_CAST "answer")) {
			robosay::base::Answer answer;
			if(answer.init(xml, ptr) == false){
				ELOGGER->print(true, "QA::init Answer format error!!!\n");
				return false;
			}
			if(answer.getType() == "0"){
                m_vWebAnswer.push_back(answer);
			}else if(answer.getType() == QA_IVRWay){
				m_vIVRAnswer.push_back(answer);			
			}
			else{
                m_vWecAnswer.push_back(answer);
			}
		}else if(!xmlStrcmp(ptr->name, BAD_CAST "jump")) {
			assert(!m_target);
			m_target = new robosay::base::Target();
			if(!m_target){
				return false;
			}
			if(m_target->parse(xml.getValue(ptr)) == false){
				std::cout<<"Target format error! ["<<xml.getValue(ptr)<<"]"<<std::endl;
				return false;
			}
		}
		else if(!xmlStrcmp(ptr->name, BAD_CAST "keyword")){
			robosay::KeyWordAnalyse keyWordAnalyse;
			std::vector<std::string> dicKeyWord; 
			
			const std::vector< std::vector<std::string> >&  vecVec = keyWordAnalyse.analyse(xml.getValue(ptr),dicKeyWord,m_wordProperty);
			if(vecVec.size() == 0)
				return false;
			for(size_t nsize=0;nsize<vecVec.size();nsize++){
				robosay::general::Sentence sentence; 
				sentence.setWords(vecVec[nsize]);
				m_vKeyWord.push_back(sentence);
			}

			m_dicKeyWord.push_back(dicKeyWord);
		}
		ptr = ptr->next;
	}

	if(m_mSemantics.empty() && m_vQuestion.empty()){
		ELOGGER->print(true, "QA::init [m_seSemantic.empty() && m_vQuestion.empty()]Every <qa> node must has one <semantic> or <qeustion> child!");
		return false;
	}

	if(m_vWecAnswer.empty() && m_vWebAnswer.empty() && m_vIVRAnswer.empty() && (!m_target || m_target->empty())){
		std::string tempQuestion;
		if(m_vQuestion.size()>0){
			tempQuestion = m_vQuestion[0].getSentence();
		}
		//ELOGGER->print(true, "QA::init [m_vAnswer.empty()]m_vAnswer Every <qa> node must has one <answer> or <jump> child!");
		ELOGGER->print(true, "QA has no answer.[%s]or[%s] node must has one <answer>!",semanticStr.c_str(),tempQuestion.c_str());		
		return false;
	}

	return true;
}

bool QA::init(const std::string& questionValue, const std::string& anwserValue){
	
	robosay::general::Sentence sentence;
	acl::String sQuestion = questionValue;		
	sQuestion.toUpper();
	sQuestion.removeChars("\t\r\n");			
	sentence.setSentence(sQuestion, "");
	m_vQuestion.push_back(sentence);

	robosay::base::Answer answer;
	if(answer.init(anwserValue) == false){
		ELOGGER->print(true, "QA::init Answer format error!!!\n");
		return false;
	}
	if(answer.getType() == "0"){
	    m_vWebAnswer.push_back(answer);
	}else if(answer.getType() == QA_IVRWay){
		m_vIVRAnswer.push_back(answer);			
	}
	else{
	    m_vWecAnswer.push_back(answer);
	}


	if(m_vQuestion.empty()){
		ELOGGER->print(true, "QA::init m_vQuestion.empty()]Every <qa> node must has one <qeustion> child!");
		return false;
	}

	if(m_vWecAnswer.empty() && m_vWebAnswer.empty() && m_vIVRAnswer.empty() ){
		std::string tempQuestion;
		if(m_vQuestion.size()>0){
			tempQuestion = m_vQuestion[0].getSentence();
		}
		//ELOGGER->print(true, "QA::init [m_vAnswer.empty()]m_vAnswer Every <qa> node must has one <answer> or <jump> child!");
		ELOGGER->print(true, "QA has no answer.[%s] node must has one <answer>!",tempQuestion.c_str());		
		return false;
	}

	return true;
}


void QA::SetKeyWord(const std::string& strKeyWord)
{
	robosay::KeyWordAnalyse keyWordAnalyse;
	std::vector<std::string> dicKeyWord; 
	const std::vector< std::vector<std::string> >&  vecVec = keyWordAnalyse.analyse(strKeyWord,dicKeyWord,m_wordProperty);
	
 	if(vecVec.size() == 0){ return; }
	
	for(size_t nsize=0;nsize<vecVec.size();nsize++)
	{
		robosay::general::Sentence sentence; 
		sentence.setWords(vecVec[nsize]);
		m_vKeyWord.push_back(sentence);
	}

	m_dicKeyWord.push_back(dicKeyWord);	
}

float QA::matchByTags(const std::set<std::string>& tags, std::set<std::string>& setMatchTag) const{
	//std::set<std::string> seMatched;
	std::set<std::string>::const_iterator iter;
	float nScore = 0.0;

	if(tags.size()==0 || m_defineTags.size()==0){
		return nScore;
	}
	
	set_intersection(m_defineTags.begin(),m_defineTags.end(),tags.begin(),tags.end(),std::inserter(setMatchTag,setMatchTag.begin()));
	if(setMatchTag.empty()){
		return nScore;
	}

	nScore = setMatchTag.size();
	
	nScore = nScore*2/(m_defineTags.size()+tags.size());
	
	return nScore;
}

/*
float QA::matchByTfidf(const std::vector<int>& vWordIdx, TFIDF* pTFIDF, int& nIdx){
	float fMaxScore = 0.0;

	for(size_t i=0; i<m_vQuestion.size(); i++){
		float fScore = m_vQuestion[i].matchByTfidf(vWordIdx,pTFIDF);
		if(fScore > fMaxScore){
			fMaxScore = fScore;
			nIdx = i;
		}
	}

	for(size_t i=0; i<m_vKeyWord.size(); i++){
		float fScore = m_vKeyWord[i].matchByTfidf(vWordIdx,pTFIDF);
		if(fScore > fMaxScore){
			fMaxScore = fScore;
			nIdx = m_vQuestion.size()+i;
		}
	}

	return fMaxScore;
}
*/
float QA::matchByTfidf(const std::vector<std::vector<TfidfValue> >& vInTfidf, int& nIdx,const CTfIdfRemend& tfIdfRemend){
	float fMaxScore = 0.0;

	for(size_t i=0; i<m_vQuestion.size(); i++){
		float fScore = m_vQuestion[i].matchByTfidf(vInTfidf,tfIdfRemend);
		if(fScore > fMaxScore){
			fMaxScore = fScore;
			nIdx = i;
		}
	}

	for(size_t i=0; i<m_vKeyWord.size(); i++){
		float fScore = m_vKeyWord[i].matchByTfidf(vInTfidf,tfIdfRemend);
		if(fScore > fMaxScore){
			fMaxScore = fScore;
			nIdx = m_vQuestion.size()+i;
		}
	}

	return fMaxScore;
}

float QA::matchByLD(const std::string& sIn, int& nIdx){
	float fMaxScore = 0;

	for(size_t i=0; i<m_vQuestion.size(); i++){
		float fScore = m_vQuestion[i].matchByLD(sIn);
		if(fScore > fMaxScore){
			fMaxScore = fScore;
			nIdx = i;
		}
	}

	return fMaxScore;
}

float QA::matchByPinyin(const std::vector<std::string>& vWordPinyin, int& nIdx){
	float fMaxScore = 0;

	for(size_t i=0; i<m_vQuestion.size(); i++){
		float fScore = m_vQuestion[i].matchByPinyin(vWordPinyin);
		if(fScore > fMaxScore){
			fMaxScore = fScore;
			nIdx = i;
		}
	}

	return fMaxScore;
}

Answer_info QA::getAnswerInfo(const StringStringMap& mSmtcWords, const StringStringMap* mPre, const std::string& sWay)
{
	Answer_info answerInfo = base::QA::getAnswerInfo(mSmtcWords,mPre,sWay);

	if (!m_mSemantics.empty())
	{
		answerInfo.SetTitle(m_mSemantics.begin()->first);
	}

	return answerInfo;
}


std::set<std::string> QA::getTagsInSentence(const std::string& sentence){
	acl::String Sentence1(sentence);
	std::set<std::string>	result;
	std::vector<acl::String> words = Sentence1.separate_(" ");
	for(int i=0;i<words.size();i++){
		if(!words[i].empty())
			result.insert(words[i]);
	}

	return result;
}

void QA::getTitleSameWords(const std::set<int>& src,std::set<int>& result){
	if(src.size()==0 || m_titleSearchWords.size()==0){
		return ;
	}
	
	set_intersection(m_titleSearchWords.begin(),m_titleSearchWords.end(),src.begin(),src.end(),std::inserter(result,result.begin()));
	
}

void QA::getDetailSameWords(const std::set<int>& src,std::set<int>& result){


	if(src.size()==0 || m_detailSearchWords.size()==0){
		return ;
	}
	
	set_intersection(m_detailSearchWords.begin(),m_detailSearchWords.end(),src.begin(),src.end(),std::inserter(result,result.begin()));
	
}

}//namespace general
}//namespace robosay
