#include "CustomerServiceChat/FaqChat/KnowledgeBase.h"
#ifndef PRINT_LOG
#define PRINT_LOG
#endif
#include "Common/lib5/WHL_LogInstance.hpp"

namespace robosay{
namespace faq{
KnowledgeBase::KnowledgeBase(const int nIdx)
	: robosay::base::KnowledgeBase("FAQ", nIdx)
	, m_Entry()
	, m_vQA(){
}

KnowledgeBase::~KnowledgeBase(){

}

bool KnowledgeBase::readKB(robosay::XmlParser& xml){
	// 获取名为scml的根节点
	xmlNodePtr rootNode = xml.getRootNode("scml");
	if(rootNode == NULL){
		ELOGGER->print(true, "KnowledgeBase::readKB [xml.getRootNode('scml') == NULL]: Failed to get root node.");
		return false;
	}
	xmlNodePtr qaPtr = xml.getFirstChildNode(rootNode,"qa");
	while(qaPtr != NULL) {
		// 若不是元素节点则看下一个
		if(qaPtr->type != XML_ELEMENT_NODE){
			qaPtr = qaPtr->next;
			continue;
		}

		robosay::faq::QA qa(m_vQA.size());
		if(qa.init(xml,qaPtr) == false) {
			ELOGGER->print(true, "KnowledgeBase::readKB [qa.init(xml,qaPtr) == false]: Failed to init QA node.");
			return false;
		}
		m_vQA.push_back(qa);
		qaPtr = qaPtr->next;
	}

	return true;
}

void KnowledgeBase::makeEntry() {
	const std::vector<robosay::faq::QA>::const_iterator last = m_vQA.end();
	std::vector<robosay::faq::QA>::const_iterator itr = m_vQA.begin();
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

bool KnowledgeBase::chat(robosay::base::Input& input,robosay::base::VResult& vResult, const std::string& sWay){
	//依次进行上次对话还原
	if(input.m_vResult.size() != 1){
		//return false;
	}
	IntSet seIdx = m_Entry.getIdx(input.m_vResult[0]->getSmtcWords());

	const IntSet::const_iterator last = seIdx.end();
	IntSet::const_iterator itr = seIdx.begin();
	for(; last != itr; ++itr){
		robosay::base::VResult vMidRslt;
		m_vQA[*itr].match(input.m_vResult[0],vMidRslt, sWay);
		if(!vMidRslt.empty()){
			if (input.m_vResult[0]->getAnalyze() != "NULL" && !input.m_vResult[0]->getAnalyze().empty()){
				const robosay::base::VResult::const_iterator l = vMidRslt.end();
				robosay::base::VResult::const_iterator i = vMidRslt.begin();
				for(; l != i; ++i){
					if ((*i)->getAnalyze().empty()){
						(*i)->setAnalyze(input.m_vResult[0]->getAnalyze());
					}
				}
			}
			insertKBIdx(vMidRslt);

			std::vector<robosay::base::Result*>::const_iterator itrResult = vMidRslt.begin();
			const std::vector<std::string>& vRets = getRelateQuestion(*itr);
			for(;itrResult!=vMidRslt.end();itrResult++){
				(*itrResult)->setRelateQuestions(vRets);
			}
				
			vResult.moveFrom(vMidRslt);

		}
	}

	/*if(vResult.empty()){
		StringVector vAnswers = m_Config.m_basicSearch.searchAnswers(input.m_sIn);
		if(!vAnswers.empty()){
			robosay::base::Result* pRst = m_vQA[0].createResult();
			if(!pRst){
				return false;
			}
			srand(time(NULL));
			int nIdx = rand() % vAnswers.size();
			pRst->m_pPath->setPath(NULL);
			pRst->calcScore(NULL);
			vResult.push_back(pRst);
			insertKBIdx(vResult);
		}
	}*/

	return true;
}

std::vector<std::string> KnowledgeBase::getRelateQuestion(const int cur_idx){
        StringVector vRets;

        if(m_kbInfo.get_level() != "2"){
            return vRets;
        }

    	const size_t uCnt = m_vQA.size();
    	for(size_t uIdx = 0u; uIdx < uCnt; ++uIdx){
            if(uIdx != cur_idx){
					const std::string&  result = m_vQA[uIdx].getRelateQuestion();
					if(!result.empty())
				    	vRets.push_back(result);		
            }
    	}
		
    	return vRets;
}

}//namespace faq
}//namespace robosay

