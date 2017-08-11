#include "CustomerServiceChat/QA.h"
#include "CustomerServiceChat/Path.h"
#include "Common/lib4/ScopedPtr.h"
#include "Common/lib4/Utility.hpp"
#include "Common/GlobalOption.h"
#include "Common/lib5/WHL_LogInstance.hpp"

const std::string robosay::base::QA::QA_IVRWay = "2";

namespace robosay{
namespace base{

QA::QA(const int nIdx)
	: m_nIdx(nIdx)
	, m_mSemantics()
	, m_pairQuestion()
	, m_target(NULL)
	, m_vWebAnswer()
	, m_vWecAnswer()
	, m_vIVRAnswer()	{
}

QA::~QA(){
	if(m_target){
		delete m_target;
	}
}

bool QA::init(const robosay::XmlParser& xml, xmlNodePtr qaPtr, const robosay::UserDicParser* pUDParser){
	xmlNodePtr ptr = xml.getFirstChildNode(qaPtr);
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
		}else if(!xmlStrcmp(ptr->name, BAD_CAST "question")) {
			m_pairQuestion.first = xml.getAttribute(ptr, "id");
			m_pairQuestion.second = xml.getValue(ptr);
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
			else
			{
                m_vWecAnswer.push_back(answer);
			}
		}else if(!xmlStrcmp(ptr->name, BAD_CAST "jump")) {
			assert(!m_target);
			m_target = createTarget();
			if(!m_target){
				return false;
			}
			if(m_target->parse(xml.getValue(ptr)) == false){
				ELOGGER->print(true, "QA::init Failed to call m_target->parse!!!\n");
				std::cout<<"Target format error! ["<<xml.getValue(ptr)<<"]"<<std::endl;
				return false;
			}
		}
		ptr = ptr->next;
	}

	bool bNoQuestion = m_mSemantics.empty() && m_pairQuestion.second.empty();
    bool bNoAnswer = m_vWecAnswer.empty() && m_vWebAnswer.empty() && m_vIVRAnswer.empty() && (!m_target || m_target->empty());

	if(bNoQuestion || bNoAnswer){
			ELOGGER->print(true, "QA has no answer.[%s]or[%s] node must has one <answer>!",semanticStr.c_str(),m_pairQuestion.second.c_str());		
            //ELOGGER->print(true, "QA::init [No question or no answer!!!\nEvery <qa> node must has one <semantic> or <qeustion> child!!!\n");
            return false;
	}

	return true;
}

std::string QA::toStr() const{
	std::string sRet;
	return sRet;
}

bool QA::match(const robosay::base::Result* oAsk,robosay::base::VResult& vRsts, const std::string& sWay,const std::map<std::string,std::string>* mPreSmtc/* = NULL*/) const{
	robosay::base::Result* pRst = createResult();
	if(!pRst){
		ELOGGER->print(true, "QA::match createResult() is failed!!!\n");
		return false;
	}
	scoped_ptr<robosay::base::Result> autoPtrObj(pRst);
	const StringStringMap mSemantic = oAsk->getSmtcWords();

	StringStringMap::const_iterator itr = m_mSemantics.begin();
	const StringStringMap::const_iterator last = m_mSemantics.end();
	int nMatchedAsk = 0, nMatchedPre = 0;
	for(; last != itr; ++itr){
		StringStringMap::const_iterator itrFind = mSemantic.find(itr->first);
		if(itrFind == mSemantic.end()){
			ELOGGER->print(true, "QA::match *itr is not find in mSemantic!!!\n");
			if(!mPreSmtc){
				return false;
			}
			itrFind = mPreSmtc->find(itr->first);
			if(itrFind == mPreSmtc->end()){
				ELOGGER->print(true, "QA::match *itr is not find in mPreSmtc!!!\n");
				return false;
			}
			else{
				++nMatchedPre;
				pRst->insertSmtcWord(itrFind->first, itrFind->second, itr->second);
			}
		}
		else{
			++nMatchedAsk;
			pRst->insertSmtcWord(itrFind->first, itrFind->second, itr->second);
		}
	}
    if(!nMatchedAsk){
        return false;
    }
	if(m_target && !m_target->empty()){
		robosay::base::VResult v;
		bool bRet = getJumpResult(oAsk, v,mPreSmtc, *m_target, sWay);
		if(!v.empty()){
			vRsts.moveFrom(v);
		}
	}else{
		pRst->m_pPath->setPath(this);
		/*float fFull = mSemantic.size() * 5;
		if(mPreSmtc){
			fFull += mPreSmtc->size();
		}
		pRst->m_fScore = (nMatchedAsk * 5 + nMatchedPre) / fFull;*/
        pRst->calcScore(NULL);
        Answer_info answerInfo = getAnswerInfo(mSemantic, mPreSmtc, sWay);
        std::string sAnswer = answerInfo.get_answer();
        size_t uPos = sAnswer.find("analyze:");
        if(uPos != std::string::npos){
            pRst->setAnalyze(sAnswer.substr(uPos + 8));
            answerInfo.set_answer(sAnswer.substr(0, uPos));
        }
        pRst->setAnswerInfo(answerInfo);
	}

	if(pRst->isValid()){
		autoPtrObj.detach();
		vRsts.push_back(pRst);
	}

	return true;
}

bool QA::make_answer(robosay::base::VResult& vRsts, const std::string& sWay) const{
	robosay::base::Result* pRst = createResult();
	if(!pRst){
		ELOGGER->print(true, "QA::make_answer createResult is failed!!!\n");
		return false;
	}
	scoped_ptr<robosay::base::Result> autoPtrObj(pRst);
	pRst->m_pPath->setPath(this);
	pRst->calcScore(NULL);
    StringStringMap mEmpty;
    Answer_info answerInfo = getAnswerInfo(mEmpty, NULL, sWay);
    std::string sAnswer = answerInfo.get_answer();
    size_t uPos = sAnswer.find("analyze:");
    if(uPos != std::string::npos){
        pRst->setAnalyze(sAnswer.substr(uPos + 8));
        answerInfo.set_answer(sAnswer.substr(0, uPos));
    }
    pRst->setAnswerInfo(answerInfo);

	autoPtrObj.detach();
	vRsts.push_back(pRst);

	return true;
}

bool QA::getJumpResult(const robosay::base::Result* oAsk,robosay::base::VResult& vRsts,const StringStringMap* mPreSmtc, const Target& target,const std::string& sWay) const {
	const QA* pQA = getTargetQA(target);
	if(!pQA){
		ELOGGER->print(true, "QA::getJumpResult getTargetQA is failed!!!\n");
		return false;
	}

	robosay::base::Result* pRst = createResult();
	if(!pRst){
		ELOGGER->print(true, "QA::getJumpResult createResult is failed!!!\n");
		return false;
	}
	scoped_ptr<robosay::base::Result> autoPtrObj(pRst);
	StringStringMap oJumpAsk = oAsk->getSmtcWords();
	StringStringMap oJumpPre;
	if(mPreSmtc){
		oJumpPre = *mPreSmtc;
	}
	std::vector<StringStringPair>::const_iterator itr = target.m_vFromTo.begin();
	const std::vector<StringStringPair>::const_iterator last = target.m_vFromTo.end();
	for(; last != itr; ++itr){
		StringStringMap::iterator itrFind = oJumpAsk.find(itr->first);
		if(itrFind == oJumpAsk.end()){
			ELOGGER->print(true, "QA::getJumpResult itr->first is not find in oJumpAsk!!!\n");
			if(!mPreSmtc){
				return false;
			}
			itrFind = oJumpPre.find(itr->first);
			if(itrFind == oJumpPre.end()){
				ELOGGER->print(true, "QA::getJumpResult itr->first is not find in oJumpPre!!!\n");
				return false;
			}
		}
		pRst->insertSmtcWord(itr->second, itrFind->second, itr->second);
	}

	pRst->setScore(1.f);
	pRst->m_pPath->setPath(pQA);
    Answer_info answerInfo = pQA->getAnswerInfo(pRst->getSmtcWords(), &oJumpPre, sWay);
    std::string sAnswer = answerInfo.get_answer();
    size_t uPos = sAnswer.find("analyze:");
    if(uPos != std::string::npos){
        pRst->setAnalyze(sAnswer.substr(uPos + 8));
        answerInfo.set_answer(sAnswer.substr(0, uPos));
    }
    pRst->setAnswerInfo(answerInfo);
	autoPtrObj.detach();
	vRsts.push_back(pRst);
	return true;
}

Answer_info QA::getAnswerInfo(const StringStringMap& mSmtcWords, const StringStringMap* mPre, const std::string& sWay) const{
    const std::vector<robosay::base::Answer>* pvAnswer = NULL;
    if(sWay == "0"){ //web
        if(m_vWebAnswer.empty()){
			if(m_vWecAnswer.empty()){
            	pvAnswer  = &m_vIVRAnswer;
			}
			else{
				pvAnswer = &m_vWecAnswer;
			}
        }else
        {
            pvAnswer = &m_vWebAnswer;
        }
     }else	if(sWay == QA_IVRWay){
     	if(!m_vIVRAnswer.empty()){
			pvAnswer = &m_vIVRAnswer;
 		}
        else if(m_vWebAnswer.empty()){
            pvAnswer  = &m_vWecAnswer;
        }else{
            pvAnswer = &m_vWebAnswer;
        }     	
 	 }
     else{ // sWay == "1"
        if(m_vWecAnswer.empty()){
			if(m_vWebAnswer.empty()){
            	pvAnswer  = &m_vIVRAnswer;
			}
			else
			{
            	pvAnswer  = &m_vWebAnswer;
			}
        }else{
            pvAnswer = &m_vWecAnswer;
        }
     }

    int nIdx = 0;
    if(aisms::gOption.sDate.empty()){
        nIdx = rand() % pvAnswer->size();
    }

    return pvAnswer->operator[](nIdx).getAnswerInfo(mSmtcWords, mPre);
}

const QA* QA::getTargetQA(const Target& target) const{
	return NULL;
}

void QA::makeEntry(StringIntSetMap& mSmtcQAIdx)const{
	StringStringMap::const_iterator itr = m_mSemantics.begin();
	const StringStringMap::const_iterator last = m_mSemantics.end();
	for(; last != itr; ++itr){
		std::map<std::string, std::set<int> >::iterator itrFind = mSmtcQAIdx.find(itr->first);
		if(itrFind != mSmtcQAIdx.end()){
			itrFind->second.insert(m_nIdx);
		}else{
			ELOGGER->print(true, "QA::makeEntry *itr is not find in mSmtcQAIdx!!!\n");
			std::set<int> seInt;
			seInt.insert(m_nIdx);
			mSmtcQAIdx[itr->first] = seInt;
		}
	}
}

std::string  QA::getRelateQuestion()const{
	const StringStringMap::const_iterator last = m_mSemantics.end();
	const StringStringMap::const_iterator itr  =  m_mSemantics.begin();
	
	if(last != itr){
		return itr->first;
	}
	else{
		return m_pairQuestion.second;
	}
}




}//namespace base
}//namespace robosay
