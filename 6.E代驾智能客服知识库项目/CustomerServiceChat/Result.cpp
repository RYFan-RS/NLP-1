#include <string.h>
#include <math.h>

#include "CustomerServiceChat/Path.h"
#include "CustomerServiceChat/Answer.h"
#include "CustomerServiceChat/Result.h"
#include "CustomerServiceChat/InternalService.h"
#include "CustomerServiceChat/Result.h"
#include "CustomerServiceChat/VKnowledgeBase.h"
#include "Common/lib5/WHL_LogInstance.hpp"
#include "CustomerServiceChat/FaqChat/Path.h"

#include "TranslatorEx.hpp"

std::string Question_info::DefaultAnwser = "DefaultAnswer";
std::string Question_info::KeyWordAnwser = "KeyWordAnwser";
std::string Question_info::SemanticAnswer = "SemanticAnswer";

std::string Question_info::SearchByPinyinAnwser = "SearchByPinyinAnwser";
std::string Question_info::SearchByLevenshteinDistanceAnwser = "SearchByLevenshteinDistanceAnwser";

std::string Question_info::tagsAnwser = "tagsAnwser";

std::string Question_info::detailAnwser = "detailAnwser";

int robosay::base::Result::max_result_number = 100;


namespace robosay{
namespace base{

robosay::base::Result* createResult(const std::string& sType){
	const char* pStr = sType.c_str();
	assert(pStr);
	if(!strcmp(pStr, "FAQ")){
		robosay::base::Path* pPath = new robosay::faq::Path;
		if(!pPath){
			ELOGGER->print(true, "robosay::base::Result* createResult faq: new robosay::faq::Path is failed!!!\n");
			return NULL;
		}
		return new robosay::base::Result("FAQ", pPath);
	}else{
		return NULL;
	}

	return NULL;
}

Result::Result(const std::string& sType, Path* pPath)
	: m_pPath(pPath)
	, m_nScore(0)
	, m_info()
	, m_sAnalyze(""){
	
}

bool Result::isValid() const{
	if(getSmtcWords().empty()){
		return false;
	}
	return m_pPath->isValid();
}

Result::~Result(){
	if(m_pPath)
		delete m_pPath;
}

std::string Result::toStr() const{
	StringStringMap mSmtcWords = getSmtcWords();
	const StringStringMap::const_iterator last = mSmtcWords.end();
	StringStringMap::const_iterator itr = mSmtcWords.begin();
	std::string sRet = "{\"semantic\":[";
	for(;last != itr; ++itr){
		sRet += "{\"";
		sRet += itr->first;
		sRet += "\":\"";
		sRet += itr->second;
		sRet += "\"},";
	}
	if(!mSmtcWords.empty()){
		sRet.resize(sRet.length() - 1);
	}
	sRet += "],\"path\":\"";
	sRet += m_pPath->toStr();
	std::stringstream ss;
	ss<<"\",\"analyze\":\""<<m_sAnalyze<<"\",\"score\":\""<<m_nScore<<"\"}";

	sRet += ss.str();
	return sRet;
}

bool Result::parse(const json::Object& oObj){
	json::Array arr = oObj["semantic"];
	json::Array::const_iterator itr(arr.Begin());
	std::string key, val;
	StringStringMap mSmtcWords;
	for(;itr != arr.End(); ++itr){
		json::Object obj(*itr);
		json::Object::const_iterator it(obj.Begin());
		for(;it != obj.End(); ++it){
			key = it->name;
			val = Json::toString(it->element);
			StringStringMap::iterator itrFind = mSmtcWords.find(key);
			if(itrFind != mSmtcWords.end() && (itrFind->second == "NULL" || itrFind->second.empty())){
				mSmtcWords[key] = val;
			}else{
				mSmtcWords.insert(std::make_pair(key,val));
			}
		}
	}
	setSmtcWords(mSmtcWords,"");
	json::String jStr = oObj["path"];
	std::string str = jStr.Value();
	m_pPath->fromStr(str);
	jStr = oObj["analyze"];
	m_sAnalyze = jStr.Value();

	return true;
}

bool Result::calcScore(void* /*pParam*/){
    if(!isValid()){
		return false;
	}
	m_nScore = 0;
	const size_t uCnt = m_pPath->m_vIndexs.size();
	int nIdx = uCnt -1;
	for(; 0 <= nIdx; --nIdx){
		m_nScore += (m_pPath->m_vIndexs[uCnt - nIdx - 1] + 1) * pow(100, nIdx);
	}
	return true;
}

StringStringMap Result::getSmtcWords() const{
	return m_info.m_question.getSmtcWords();
}

StringStringMap& Result::getSmtcWordsRef(){
	return m_info.m_question.getSmtcWordsRef();
}

void Result::setSmtcWords(const StringStringMap& mWords, const std::string& sIDs){
	m_info.m_question.setSmtcWords(mWords);
	m_info.m_question.set_id(sIDs);
}

void Result::insertSmtcWord(const std::string& l, const std::string& r, const std::string& id){
	m_info.m_question.insertSmtcWord(l, r);
    m_info.m_question.append_id(id);
}

std::string Result::getInput() const{
	return m_info.m_question.get_source();
}

void Result::setInput(const std::string& sIn){
	m_info.m_question.set_source(sIn);
}

int Result::getScore() const{
	//std::cout<<"Result::getScore():"<<m_nScore<<std::endl;
	return m_nScore;
}

void Result::setScore(const int nScore){
	m_nScore = nScore;
	std::cout<<"Result::setScore(const int nScore):"<<m_nScore<<std::endl;
}

#define RESULT_SCORE_ACCURACY  (1000)

void Result::setScore(const float fScore){
	m_nScore = fScore * RESULT_SCORE_ACCURACY;

	if(m_nScore<0){
		std::cout<<"Result::testTemp:fScore is "<<fScore<<" m_nScore is "<<m_nScore<<std::endl;
		//assert(0);
	}
	
	std::cout<<"Result::setScore(const float fScore):"<<m_nScore<<std::endl;
}

std::string Result::getTarget() const{
	return m_info.m_question.get_target();
}

void Result::setTarget(const std::string& sTarget, const std::string& sID){
	m_info.m_question.set_target(sTarget);
	m_info.m_question.set_id(sID);
}

std::string Result::getAnswer() const{
	return m_info.m_answer.get_answer();
}

void Result::setAnswer(const std::string& sAnswer, const std::string& way, const std::string& id){
	m_info.m_answer.set_answer(sAnswer);
	m_info.m_answer.set_id(id);
	m_info.m_answer.set_way(way);
}

void Result::setAnswerInfo(const Answer_info& answer){
	m_info.m_answer = answer;
}

std::string Result::getAnalyze() const{
	return m_sAnalyze;
}

void Result::setAnalyze(const std::string& sAnalyze){
	m_sAnalyze = sAnalyze;
}

std::string Result::getKBType() const{
	return m_info.m_kb.get_type();
}

void Result::setKBType(const std::string& sType){
	m_info.m_kb.set_type(sType);
}

void Result::setRelateQuestions(const StringVector& vQuestions){
    m_info.m_relate.set_questions(vQuestions);
}

VResult::VResult(){

}

VResult::~VResult(){
	clear();
}

void VResult::clear(){
	const std::vector<robosay::base::Result*>::const_iterator last = this->end();
	std::vector<robosay::base::Result*>::iterator itr = this->begin();
	for(; last != itr; ++itr){
		if(*itr){
			delete *itr;
		}
	}
	this->std::vector<robosay::base::Result*>::clear();
}

void VResult::ClearPathInfo()
{
	const std::vector<robosay::base::Result*>::const_iterator last = this->end();
	std::vector<robosay::base::Result*>::iterator itr = this->begin();
	for(; last != itr; ++itr){
		if((NULL!=*itr) && (NULL!=(*itr)->m_pPath)){
			(*itr)->m_pPath->reset();
		}
	}	
}


bool VResult::setKBType(const std::string& sType){
	if(this->empty()){
		return false;
	}
	this->operator[](0)->setKBType(sType);
	return true;
}

std::string VResult::toStr() const{
	if(this->empty()){
		return sMessage;
	}else{
		std::string s = "{\"" + this->operator[](0)->getKBType() + "\":[";

		for(size_t i=0; i<this->size(); i++){
			s += this->at(i)->toStr() + ",";
		}

		if(this->size()){
			s.resize(s.length() - 1);
		}

		s += "]}";

		return s;
	}
}

bool VResult::fromStr(const std::string& sCmd){
#if defined(_CHAT_LOG)
	ssDebug<<"<< 待解析的json串为:"<<sCmd;
	acl::debug::MESSAGE(DSTREAM,ssDebug,acl::debug::LEVEL0);
#endif
	json::Object rootObj;
	if(Json::read(sCmd, rootObj) == false){
		ELOGGER->print(true, FAILE_CALL_PARAM("VResult::fromStr","[Json::read(sCmd, rootObj) == false]",sCmd.c_str()));
		return false;
	}
	this->clear();
	json::Object::iterator itr = rootObj.Begin();
	const json::Object::const_iterator last  = rootObj.End();
	for(; last != itr; ++itr){
		const std::string& sType = itr->name;
		json::Array& jarr = itr->element;
		json::Array::iterator it = jarr.Begin();
		json::Array::const_iterator la = jarr.End();
		for(; la != it; ++it){
			const json::Object& obj = (*it);
			Result* newRst = createResult(sType);
			if(!newRst){
				return false;
			}
			if(!newRst->parse(obj)){
				delete newRst;
				return false;
			}
			this->push_back(newRst);
		}
	}

	return true;
}


void VResult::sort(){
	std::sort(this->begin(),this->end(), RstCompare);
}

void VResult::moveFrom(VResult& rhs){
	const std::vector<robosay::base::Result*>::const_iterator last = rhs.end();
	std::vector<robosay::base::Result*>::iterator itr = rhs.begin();
	for(; last != itr; ++itr){
		if(*itr){
			this->push_back(*itr);
#ifdef PRINT
            StringStringMap mSmtcWords = (*itr)->getSmtcWords();
            if(!mSmtcWords.empty()){
				static int i=1;
				StringStringMap::iterator it = mSmtcWords.begin();
				std::cout<<i<<mSmtcWords.begin()->first<<"-"<<mSmtcWords.begin()->second<<std::endl;
				++i;
			}
#endif // PRINT
		}
	}
	rhs.std::vector<robosay::base::Result*>::clear();
}

void VResult::reduceSize(const size_t uSize){
	if(uSize >= this->size()){
		return;
	}

	for(size_t uIdx = uSize; uIdx < this->size(); ++uIdx){
		robosay::base::Result* newRslt = this->operator[](uIdx);
		if(newRslt){
			delete newRslt;
			this->operator[](uIdx) = NULL;
		}
	}

	this->std::vector<robosay::base::Result*>::resize(uSize);
}

int VResult::getKBIdx() const{
	if(this->empty()){
		ELOGGER->print(true, "VResult::getKBIdx this->empty()\n");
		return -1;
	}
	const Result* rst = this->operator[](0);
	if(!rst || !rst->m_pPath){
		ELOGGER->print(true, "VResult::getKBIdx  [!rst||!rst->m_pPath]\n");
		return -1;
	}
	return rst->m_pPath->getKBIdx();
}

bool VResult::getResultInfo(std::string& sContext, Result_info& resultInfo,const std::map<int,std::string>& mapIdToWord) const{
    if(this->empty()){
        ELOGGER->print(true, "VResult::getAnswer this->empty()\n");
        return false;
    }

    const Result* pResult = this->operator[](0);

	#if 0
    std::string sRet = pResult->getAnswer();
    if(sRet.find("CALL_EXTERN_API:C++:") != std::string::npos){
        std::string sTmp = sRet;
        sRet = executeExternalAPI(sTmp, _extAPI);
        if(sRet.find("ERROR:") != std::string::npos){
            std::cout<<"failed to call m_extAPI.process for source = ["<<sTmp<<"]"<<std::endl;
            std::cout<<"error log = ["<<sRet<<"]"<<std::endl;
            //return false;
            sRet = "Sorry, ExternalAPI不能正常工作了，请稍候再试！！！";
        }
    }

	
	Answer_info answerInfo = pResult->getAnswerInfo();

	std::string answerContent = acl::Utility::int2Str(this->size());
	for(int i=0;i<this->size();i++){
		answerContent+=(";"+this->operator[](i)->getAnswer());
	}

	answerInfo.setAnswerContent(answerContent);

	(const_cast<Result*>(pResult))->setAnswerInfo(answerInfo);
	
    resultInfo.m_answer = pResult->getAnswerInfo();
    //resultInfo.m_answer.set_answer(answerContent);
    resultInfo.m_question = pResult->getQuestionInfo();

	Relate_info relateInfo = pResult->getRelateInfo();
	if(!relateInfo.get_questions().empty()){
		resultInfo.m_relate = relateInfo;
	}
	resultInfo.m_iScore = pResult->getScore();
	#endif

	std::stringstream ss;
	ss<<"<answer_info>";


	for(int i=0;i<this->size();i++){
		ss<<"<answer>";
		ss<<"<id><![CDATA["<<this->operator[](i)->getAnswer()<<"]]></id>";

		ss<<"<type><![CDATA["<<this->operator[](i)->getQuestionInfo().get_type()<<"]]></type>";
		if(this->operator[](i)->getQuestionInfo().get_type()==Question_info::tagsAnwser){
			ss<<"<tag><![CDATA["<<this->operator[](i)->getTarget()<<"]]></tag>";
		}else{
			ss<<"<tag><![CDATA["<<"]]></tag>";		
		}

		std::string text;
		std::set<int>::const_iterator itr = this->operator[](i)->m_sMatchTitleWords.begin();
		std::set<int>::const_iterator itrEnd = this->operator[](i)->m_sMatchTitleWords.end();
		for(;itr != itrEnd;itr++){
			std::map<int,std::string>::const_iterator it = mapIdToWord.find(*itr);
			if(it!=mapIdToWord.end()){
				text+= it->second;
				text+= ";";
			}
		}
		ss<<"<title><![CDATA["<<text<<"]]></title>";	

		text = "";
		if(this->operator[](i)->getQuestionInfo().get_type()==Question_info::detailAnwser){
			text = this->operator[](i)->getTarget();
		}
		else{
			text = this->operator[](i)->m_sMatchDetailWords;
			/*itr = this->operator[](i)->m_sMatchDetailWords.begin();
			itrEnd = this->operator[](i)->m_sMatchDetailWords.end();
			for(;itr != itrEnd;itr++){
				std::map<int,std::string>::const_iterator it = mapIdToWord.find(*itr);
				if(it!=mapIdToWord.end()){
					text+= it->second;
					text+= ";";
				}
			}*/
		}
		ss<<"<content><![CDATA["<<text<<"]]></content>";	
		ss<<"</answer>";
	}
	
	ss<<"</answer_info>";

	sContext = ss.str();
	
	std::string answerContent;

	Answer_info answerInfo = pResult->getAnswerInfo();
	
	answerInfo.setAnswerContent(answerContent);

	(const_cast<Result*>(pResult))->setAnswerInfo(answerInfo);
	
    resultInfo.m_answer = pResult->getAnswerInfo();
    //resultInfo.m_answer.set_answer(answerContent);
    resultInfo.m_question = pResult->getQuestionInfo();

	Relate_info relateInfo = pResult->getRelateInfo();
	if(!relateInfo.get_questions().empty()){
		resultInfo.m_relate = relateInfo;
	}	
	resultInfo.m_iScore = pResult->getScore();

    return true;
}
void VResult::selectBest(){
	selectServiceScenario();

	std::sort(this->begin(),this->end(), RstSemanticCompare);
}

void VResult::selectServiceScenario(){
	VResult newResult;
	bool hasService = false;

	VResult::iterator it = this->begin();
	for(;it!=this->end();it++){
		std::string serviceTag = (*it)->getQuestionInfo().hasServiceSemantic();
		if(!serviceTag.empty()){
			hasService = true;
		}
	}

	if(!hasService) return;

	it = this->begin();
	
	for(;it!=this->end();){
		std::string serviceTag = (*it)->getQuestionInfo().hasServiceSemantic();
		if(serviceTag.empty()){
			robosay::base::Result* delResult = (*it);
			it = this->erase(it);
			delete delResult;
		}
		else{
			it++;
		}
	}	
}

std::string VResult::executeExternalAPI(const std::string& sCmd, robosay::base::ExternalAPI_info* _extAPI) const{
    if(!_extAPI->m_pExtClient){
		ELOGGER->print(true, "in VResult::executeExternalAPI _extAPI is NULL\n");
        return "Failed to call ExternalAPI to translate result!";
    }
    std::string sAnswer;
	_extAPI->m_transport->open();
    _extAPI->m_pExtClient->process(sAnswer, sCmd);
	_extAPI->m_transport->close();
    if(!sAnswer.empty() && sAnswer.find("ERROR") == std::string::npos){
        return sAnswer;
    }
    return "Failed to call ExternalAPI to translate result!";
}

bool VResult::PathInVResult(robosay::base::Result* result) const{
	if(result==NULL || result->m_pPath==NULL)
		return false;
	
	for(size_t uIdx = 0; uIdx < this->size(); ++uIdx){
		robosay::base::Result* newRslt = this->operator[](uIdx);
		if(newRslt){
			if(newRslt->m_pPath){
				if(newRslt->m_pPath->m_vIndexs.size()==result->m_pPath->m_vIndexs.size()){
					int i =0;
					for( i=0;i<newRslt->m_pPath->m_vIndexs.size();i++){
						if(newRslt->m_pPath->m_vIndexs[i] != result->m_pPath->m_vIndexs[i])
							break;
					}
					if(newRslt->m_pPath->m_vIndexs.size()!=0 && (i==newRslt->m_pPath->m_vIndexs.size())){
						return true;
					}
				}
			}
		}
	}

	return false;
}

}//namespace base
}//namespace robosay
