#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "MeanResult.h"

MeanResult::MeanResult()
{
    //mp_DataManager = NULL;
    mp_NormalRuleManager = NULL;
    m_start = -1;
    m_end = -1;
}

MeanResult::~MeanResult()
{
}

void MeanResult::clear()
{
    m_start = -1;
    m_end = -1;
    m_keyRanges.clear();
    m_tagsPerWord.clear();
    m_deletedTagsPerWord.clear();
}

void MeanResult::calculateRange()
{
    size_t i,size=m_keyRanges.size();

    m_start = TAIL;

    for(i=0; i<size; i++) {
        if(m_keyRanges[i].start == -1) {
            continue;
        }
        if(m_keyRanges[i].start < m_start) {
            m_start = m_keyRanges[i].start;
        }
        if(m_keyRanges[i].end> m_end) {
            m_end = m_keyRanges[i].end;
        }
    }
    //std::cout<<"+-MeanResult:"<<m_start<<"--"<<m_end<<std::endl;
}

void MeanResult::setRuleEntry(const sae::ENTRY& entry)
{
    m_ruleEntry = entry;
}

std::string MeanResult::getRuleName()
{
    return mp_DataManager->getIndexManager().find(m_ruleEntry.m_index);
}

void VMeanResult::dump(std::ostream& ofs) const
{
    const int nEnd = this->size() -1 ;
    for(size_t i=0; i<this->size(); i++) {
        (*this)[i].dump(ofs);
        if(i != nEnd) ofs<<std::endl;
    }
}

void VMeanResult::getRuleIdNameVec(std::vector<std::string>& vRuleIdName) const
{
    for(size_t i=0; i<this->size(); i++) {
        std::string ruleIdName  = (*this)[i].getRuleIdName();
		if(!ruleIdName.empty()){
			vRuleIdName.push_back(ruleIdName);
		}
    }
}

std::multimap<std::string,std::string> VMeanResult::getTagContentMap(const sae::MeanWords& words) const{
	std::multimap<std::string,std::string> mTagContent;
	std::multimap<std::string,std::string>::const_iterator iter;
	
	for(size_t i=0; i<this->size(); i++){
		std::multimap<std::string,std::string> mTemp = (*this)[i].getTagContentMap(words);
		for(iter=mTemp.begin(); iter!=mTemp.end(); ++iter){
			mTagContent.insert(std::make_pair(iter->first,iter->second));
		}
	}
	
	return mTagContent;
}

const std::string MeanResult::dumpTag2Str(const bool& color)
{
    std::string s;
    size_t j;
    std::string hColor="";
    std::string tColor="";

    if(color) {
        hColor="\033[1;35m";
        tColor="\033[0m";
    }

    if(!m_tagsPerWord.empty()) {
        for(int i=m_start; i<=m_end; i++) {
            bool first=true;
            s += sae::Utility::int2Str(i) + ":";
            for(j=0; j<m_tagsPerWord[i].size(); j++) {
                if(!first) {
                    s += ";";
                }
                if(mp_DataManager == NULL) {
                    s += sae::Utility::int2Str(m_tagsPerWord[i][j]);
                } else {
                    s += hColor + mp_DataManager->getTagManager().find(m_tagsPerWord[i][j]) + tColor;
                }
                first=false;
            }
            if(!m_deletedTagsPerWord.empty()) {
                for(j=0; j<m_deletedTagsPerWord[i].size(); j++) {
                    if(!first) {
                        s += ";";
                    }
                    if(mp_DataManager == NULL) {
                        s += "!" + sae::Utility::int2Str(m_deletedTagsPerWord[i][j]);
                    } else {
                        s += hColor + "!" + mp_DataManager->getTagManager().find(m_deletedTagsPerWord[i][j]) + tColor;
                    }
                    first=false;
                }
            }
            if(i != m_end) {
                s += "|";
            }
        }
    }

    if(!m_sentenceTags.empty()) {
        s += " [S]:";
        for(j=0; j<m_sentenceTags.size(); j++) {
            if(j != 0) {
                s += ";";
            }
            if(mp_DataManager == NULL) {
                s += sae::Utility::int2Str(m_sentenceTags[j]);
            } else {
                s += hColor + mp_DataManager->getTagManager().find(m_sentenceTags[j]) + tColor;
            }
        }
    }

    return s;
}

int MeanResult::getRuleLineNum()
{
    return mp_NormalRuleManager->getRule(m_ruleEntry.m_index).getLineIndexID();
}

std::string MeanResult::getLineIndexInfo()
{
    int lineIndexID = getRuleLineNum();
    return mp_DataManager->getLineIndexManager().dumpLineIndexInfo(lineIndexID);
}

std::string MeanResult::getTagNameAt(const int& i, const int& j)
{
    if(i<(int)m_tagsPerWord.size()) {
        if(j<(int)m_tagsPerWord[i].size()) {
            if(mp_DataManager != NULL) {
                return mp_DataManager->getTagManager().find(m_tagsPerWord[i][j]);
            } else {
                return sae::Utility::int2Str(m_tagsPerWord[i][j]);
            }
        }
    }
    return "";
}

void MeanResult::setKeyRanges(const sae::VRANGE& vRange)
{
    m_keyRanges = vRange;
}

void MeanResult::addKeyRange(const sae::RANGE& range)
{
    m_keyRanges.push_back(range);
}

void MeanResult::dump(std::ostream& ofs) const
{
    if(mp_DataManager == NULL) {
        ofs<<"Index:"<<m_ruleEntry.m_indexRuleID<<DUMP_SEPARATOR;
    } else {
        ofs<<"Rule Name:"<<mp_DataManager->getConstIndexManager().find(m_ruleEntry.m_indexRuleID)<<DUMP_SEPARATOR;
		ofs<<"Rule Id:"<<getRuleIdName()<<DUMP_SEPARATOR;
    }
    ofs<<"Range:"<<m_start<<"--"<<m_end<<DUMP_SEPARATOR;

    if(!m_tagsPerWord.empty()) {
        ofs<<"Tags:";
        for(int i=m_start; i<=m_end; i++) {
            ofs<<i<<":";
            for(size_t j=0; j<m_tagsPerWord[i].size(); j++) {
                if(j != 0) {
                    ofs<<";";
                }
                if(mp_DataManager == NULL) {
                    ofs<<m_tagsPerWord[i][j];
                } else {
                    ofs<<mp_DataManager->getConstTagManager().find(m_tagsPerWord[i][j]);
                }
            }
            if(i != m_end) {
                ofs<<"|";
            }
        }
		ofs<<DUMP_SEPARATOR;
    }
	
	if(!m_sentenceTags.empty()){
		ofs<<"[S]:";
		for(size_t i=0; i<m_sentenceTags.size(); i++){
			if(mp_DataManager == NULL) {
				ofs<<m_sentenceTags[i];
			} else {
				ofs<<mp_DataManager->getConstTagManager().find(m_sentenceTags[i]);
			}
			if(i != m_sentenceTags.size()-1) {
                ofs<<"|";
            }
		}
	}
	
    if(!m_sTemplates.empty()) {

        for(size_t i=0; i<m_sTemplates.size(); i++) {
            ofs<<DUMP_SEPARATOR;
            ofs<<"Template["<<i<<"]:"<<m_sTemplates[i];
        }
    }
	
	ofs<<DUMP_SEPARATOR;
	ofs<<mp_DataManager->getConstLineIndexManager().dumpLineIndexInfo(m_ruleEntry.m_fileLineIdx);
}

std::multimap<std::string,std::string> MeanResult::getTagContentMap(const sae::MeanWords& words) const{
	std::multimap<std::string,std::string> mTagContent;
	
	for(int i=m_start; i<=m_end; i++) {
		for(size_t j=0; j<m_tagsPerWord[i].size(); j++) {
			if(mp_DataManager == NULL) {
				mTagContent.insert(std::make_pair("未知语义",words[i].getStrWord(*mp_DataManager)));
			} else {
				const std::string& sTag = mp_DataManager->getConstTagManager().find(m_tagsPerWord[i][j]);
				mTagContent.insert(std::make_pair(sTag,words[i].getStrWord(*mp_DataManager)));
			}
		}
	}
	
	if(!m_sentenceTags.empty()){
		for(size_t i=0; i<m_sentenceTags.size(); i++){
			if(mp_DataManager == NULL) {
				mTagContent.insert(std::make_pair("未知语义","[S]"));
			} else {
				const std::string& sTag = mp_DataManager->getConstTagManager().find(m_sentenceTags[i]);
				mTagContent.insert(std::make_pair(sTag,"[S]"));
			}
		}
	}
	
	return mTagContent;
}

sae::TagRule& MeanResult::getTagRule()
{
    return mp_NormalRuleManager->getRule(m_ruleEntry.m_index).getTagRule();
}

std::string MeanResult::getRuleIdName() const
{
    return mp_NormalRuleManager->getRule(m_ruleEntry.m_index).getIdName();
}


void MeanResult::setSentenceTags(const std::vector<int>& tags)
{
    m_sentenceTags = tags;
}
