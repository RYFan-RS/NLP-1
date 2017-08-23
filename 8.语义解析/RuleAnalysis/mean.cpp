#ifdef WIN32
#pragma warning(disable: 4786)
#else
#include <sys/time.h>
//#define MEAN_TIME_EVALUE
#endif //WIN32
#include "mean.h"

#define MAE_VERSION "Ver5.36(Build20110127-1)"

int ContextDictionary::init(std::string s_basic_rule_path, const std::string& s_special_rule_path,const std::map<std::string, std::set<std::string> >* pWords)
{
    if(s_basic_rule_path.empty()) {
        s_basic_rule_path = "../data/semantic.dat";
    }

	if(m_engine.fastSerialize(s_basic_rule_path) == false) {
        return -1;
    }

    if(m_engine.init(s_basic_rule_path) != 0) {
        return -2;
    }

    if(m_engine.loadUsrDic(s_special_rule_path, pWords) != 0){
        return -3;
    }

    return 0;
}

VMeanResult ContextDictionary::analyze(const std::string& input, sae::MeanWords& mean_words, const std::vector<sae::ExecutePhase>& vExecuteMethod)
{
    VMeanResult vMeanResult,vPhaseMeanResult;
    int lastPhaseBegin = 0;

    for (int i = 0; i < vExecuteMethod.size(); ++i) {
        vPhaseMeanResult = analyzePhase(input, mean_words, vExecuteMethod[i]);

        for (int j = 0; j < vPhaseMeanResult.size(); j++) {
            vMeanResult.push_back(vPhaseMeanResult[j]);
        }
        if(!vPhaseMeanResult.empty()) {
            vPhaseMeanResult.m_phases[0].m_start = lastPhaseBegin;
            vPhaseMeanResult.m_phases[0].m_end = vMeanResult.size() - 1;
            for(size_t k=0; k<vPhaseMeanResult.m_phases[0].size(); k++) {
                vPhaseMeanResult.m_phases[0][k].m_start += lastPhaseBegin;
                vPhaseMeanResult.m_phases[0][k].m_end += lastPhaseBegin;
            }
            lastPhaseBegin = vMeanResult.size();
            vMeanResult.m_phases.push_back(vPhaseMeanResult.m_phases[0]);
        } else {
            PhaseInfo emptyPhase;
            vMeanResult.m_phases.push_back(emptyPhase);
        }
    }

    return vMeanResult;
}


VMeanResult ContextDictionary::analyzePhase(const std::string& input, sae::MeanWords& words, const sae::ExecutePhase& phase)
{
    VMeanResult vPhaseMeanResult;
#ifdef MEAN_TIME_EVALUE
    struct timeval tBegin,tEnd;
    gettimeofday(&tBegin , NULL);
#endif //MEAN_TIME_EVALUE
    if( phase.hasCondition() ) {
        sae::MatchingMatrix matrix = words.generateMatchingMatrixFromSentenceTag();
        if( phase.matchConditon(matrix) == false ) {
            return vPhaseMeanResult;
        }
    }
    vPhaseMeanResult = m_engine.analyzePhase("", words, phase);
#ifdef MEAN_TIME_EVALUE
    gettimeofday(&tEnd , NULL);
    std::cout<<phase.start<<"-"<<phase.end<<":"<<phase.loopTime<<"=>";
    std::cout<<(tEnd.tv_sec-tBegin.tv_sec)*1000+(tEnd.tv_usec-tBegin.tv_usec)/1000<<" millisecond"<<std::endl;
#endif //MEAN_TIME_EVALUE
    return vPhaseMeanResult;
}

void ContextDictionary::intersection(int& a1,int& a2, const int& b1, const int& b2)
{
    if(a2 < b1 || a1 > b2) {
    } else if(a1<b1 && a2>=b1 && a2<=b2) {
        a1 = b1;
    } else if(a1>=b1 && a1<=b2 && a2>b2) {
        a2 = b2;
    } else if(a1<b1 && a2>b2) {
        a1 = b1;
        a2 = b2;
    } else {
    }
}

const std::vector<sae::ExecutePhase>& ContextDictionary::getExecuteMethod(const std::string& key)
{
    return getExcutableData().getConstDataManager().getConstExecuteManager().getExecuteMethod(key);
}

std::string ContextDictionary::getVersion()
{
    return MAE_VERSION;
}

std::string ContextDictionary::getDataVersion()
{
    return getExcutableData().getConstVersionManager().getVersion();
}

//std::vector<std::string> ContextDictionary::getWordsByTag(const std::string& sTag){
//	std::vector<std::string> vWord;
//
//	const int nTagCode = m_engine.getExecutableData().getDataManager().getTagManager().find(sTag);
//	if(nTagCode == -1){
//		return vWord;
//	}
//
//	const std::multimap<int,sae::ENTRY>& mmTagRuleIdxMap = m_engine.getExecutableData().getEntryManager().getTagRuleIdxMap();
//	std::pair<std::multimap<int,sae::ENTRY>::const_iterator,std::multimap<int,sae::ENTRY>::const_iterator> pairEntryIter;
//	std::multimap<int,sae::ENTRY>::const_iterator entryIter;
//
//	pairEntryIter = mmTagRuleIdxMap.equal_range(nTagCode);
//	for(entryIter=pairEntryIter.first;entryIter!=pairEntryIter.second;++entryIter){
//		const sae::NormalRule& rule = m_engine.getExecutableData().getConstNormalRuleManager().getConstRule(entryIter->second.m_index);
//		//- to-do TO-DO
//	}
//
//	return vWord;
//}
std::vector<std::string> ContextDictionary::getWordsByTag(const std::string& sTag)
{
    return m_engine.getExecutableData().getDataManager().getWordManager().getWords();
}

std::vector<std::string> ContextDictionary::getEntryWords() const{
	std::vector<std::string> vWord;
	
	const std::multimap<int,sae::ENTRY>& mWordEntries = m_engine.getConstExecutableData().getConstEntryManager().getWordEntries();
	const sae::WordManager wordManager = m_engine.getConstExecutableData().getConstDataManager().getConstWordManager();
	
	std::multimap<int,sae::ENTRY>::const_iterator iter;
	int lastWord = -1;
	for(iter = mWordEntries.begin(); iter != mWordEntries.end(); ++iter){
		if(iter->first != lastWord){
			lastWord = iter->first;
			vWord.push_back(wordManager.find(lastWord));
		}
	}
	
	return vWord;
}

std::string ContextDictionary::makeExecuteMethodByTag(const std::string& sTag)
{
    const int nTagCode = m_engine.getExecutableData().getDataManager().getTagManager().find(sTag);
    if(nTagCode == -1) {
        return "";
    }

    const std::multimap<int,sae::ENTRY>& mmTagRuleIdxMap = m_engine.getExecutableData().getEntryManager().getTagRuleIdxMap();
    std::pair<std::multimap<int,sae::ENTRY>::const_iterator,std::multimap<int,sae::ENTRY>::const_iterator> pairEntryIter;
    std::multimap<int,sae::ENTRY>::const_iterator entryIter;
    std::stringstream ssExeMethod;

    pairEntryIter = mmTagRuleIdxMap.equal_range(nTagCode);
    for(entryIter=pairEntryIter.first; entryIter!=pairEntryIter.second; ++entryIter) {
        ssExeMethod<<entryIter->second.m_indexRuleID<<"-"<<entryIter->second.m_indexRuleID<<";";
    }

    return ssExeMethod.str();
}

std::vector<std::string> ContextDictionary::getSemantics() const{
	std::vector<std::string> vSemantic;
	std::map<std::string,int>::const_iterator iter;

	const sae::TagManager& tagManager = this->getTagManager();
	
	std::map<std::string,int> mTag = tagManager.getTagMapWithAttribute("输出");
	for(iter=mTag.begin(); iter!=mTag.end(); ++iter){
		vSemantic.push_back(iter->first);
	}
	
	return vSemantic;
}

bool ContextDictionary::IsInExternalTag(const std::string& strTagName)const
{
	std::map<std::string,int>::const_iterator iter;
	const sae::TagManager& tagManager = this->getTagManager();
	
	std::map<std::string,int> mTag = tagManager.getTagMapWithAttribute("外部标签");
	for(iter=mTag.begin(); iter!=mTag.end(); ++iter)
	{
		if (iter->first == strTagName)
		{
			return true;
		}
	}
	
	return false;
}

