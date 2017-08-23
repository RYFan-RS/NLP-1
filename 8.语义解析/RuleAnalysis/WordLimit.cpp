#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "CommonParser.h"
#include "Utility.h"
#include "WordLimit.h"

namespace sae
{

WordLimit::WordLimit()
{
    m_vcell = NULL;
    mc_VCellIndex = -1;
}

void WordLimit::dump(const DataManager& dataManager, std::ostream& ofs) const
{
    if(m_vcell == NULL) ofs<<"*";
    else {
        for(size_t i=0; i<m_vcell->size(); i++) {
            if(i != 0) {
                if(m_vcell->getFlag() & OR_RELATION) ofs<<"/";
                else ofs<<"|";
            }
            (*m_vcell)[i]->dump(dataManager.getConstWordManager(),ofs);
        }
    }
}

bool WordLimit::isAllWordInSet(std::set<int>& setKey) const
{
    if(m_vcell == NULL) return false;

    KeyNum keyNum;

    if(m_vcell->getFlag() & AND_RELATION) {
        for(size_t i=0; i<m_vcell->size(); i++) {
            keyNum = (*m_vcell)[i]->getKeyNum();
            if(keyNum != KeyNum::Error) {
                if(setKey.find(keyNum.getKeyNum()) != setKey.end()) {
                    return true;
                }
            }
        }
    } else {
        for(size_t i=0; i<m_vcell->size(); i++) {
            keyNum = (*m_vcell)[i]->getKeyNum();
            if(keyNum != KeyNum::Error) {
                if(setKey.find(keyNum.getKeyNum()) == setKey.end()) {
                    return false;
                }
            } else {
                return false;
            }
        }
    }

    return true;
}

std::vector<std::string> WordLimit::getSimpleCombinedTypeWords(const DataManager& dataManager) const
{
    std::vector<std::string> v,emptyV;

    if(m_vcell == NULL) return emptyV;

    for(size_t i=0; i<m_vcell->size(); i++) {
        if( (*m_vcell)[i]->isReferKeyCell() ) {
            return emptyV;
        }
        if( (*m_vcell)[i]->isAndRelation() ) {
            if( (*m_vcell)[i]->getContent().size() != 1 ) {
                return emptyV;
            }
            v.push_back( dataManager.getConstWordManager().find((*m_vcell)[i]->getContent()[0]) );
        } else {
            std::string s;
            const SVector<int>& content = (*m_vcell)[i]->getContent();
            for(size_t j=0; j<content.size(); j++) {
                s += dataManager.getConstWordManager().find( content[j] );
            }
            v.push_back(s);
        }
    }

    return v;
}

bool WordLimit::hasUnisolatedComma(const std::string &s) const
{
    size_t p=0;

    while(1) {
        p = s.find(',', p+1);
        if(p!=std::string::npos && p > 0) {
            if(s[p-1] != ';' && s[p-1] != '/' && s[p-1] != '\t' && s[p-1] != '\\') {
                return true;
            }
        } else break;
    }
    return false;
}

bool WordLimit::parse(const std::string& sWordLimit, DataManager* pDataManger, ErrorData& errData)
{
    int index = gp_VCellCache->find(sWordLimit,WORD_DATABASE);
    if(index == -1) {
        m_vcell = new VCell;
        index = gp_VCellCache->insert(sWordLimit,m_vcell,WORD_DATABASE);
        if(m_vcell->parse(sWordLimit,pDataManger->getDefineManager(),pDataManger->getWordManager(),errData) == false) {
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
    } else {
        m_vcell = (*gp_VCellCache)[index];
    }
    mc_VCellIndex = index;

    return true;
}

void WordLimit::makeReferKeyInstant(const MeanWords& words, const VRANGE& keyRanges, const DataManager& dataManager, VCell& dynamicVCell) const
{
    SVector<int> dynmaicContent;

    for(size_t i=0; i<m_vcell->size(); i++) {
        if( (*m_vcell)[i]->isReferKeyCell() ) {
            dynmaicContent = words.generateDynamicContentFromWord(keyRanges.getWordRangeFromKeyNum((*m_vcell)[i]->getKeyNum())
                             ,(*m_vcell)[i]->getKeyNum().getSubKeyNum(),dataManager);
            dynamicVCell.push_back( (*m_vcell)[i]->makeNewCellByDynamicContent(dynmaicContent) );
        }
    }
}

int WordLimit::checkWord(const MeanWords& words,const VRANGE& vRange,RANGE& range,const VCell& dynamicVCell) const
{
    for(size_t i=0; i<vRange.size(); i++) {
        MatchingMatrix matrix = words.generateMatchingMatrixFromWord(vRange[i],(m_vcell->getFlag() & BASIC_WORD_CELL));
        if( m_vcell->forwardMatch(matrix,dynamicVCell) == true ) {
            range = matrix.getMatchedRange();
            if(range.isLegal()) {
                range.adjustOffset(vRange[i].start);
                return 0;
            }
        }
    }

    return 1;
}

int WordLimit::checkLimitWord(const MeanWords& words,const VRANGE& vRange,const VCell& dynamicVCell) const
{
    for(size_t i=0; i<vRange.size(); i++) {
        MatchingMatrix matrix = words.generateMatchingMatrixFromWord(vRange[i],(m_vcell->getFlag() & BASIC_WORD_CELL));
        if(m_vcell->allMatch(matrix,dynamicVCell) == false) {
            return 1;
        }
    }

    return 0;
}

int WordLimit::checkPhraseWord(const MeanWords& words,const VRANGE& vRange,RANGE& range,const VCell& dynamicVCell) const
{
    switch(m_vcell->getFlag()) {
    case SHUFFLE_RELATION:
        if(checkPhraseWordShuffle(words,vRange,range,dynamicVCell) == false) {
            return 1;
        }
        break;
    case OR_RELATION:
    default:
        if((*m_vcell)[0]->isNegativeSet()) {
            if(checkPhraseWordOr(words,vRange,range,dynamicVCell) == true) {
                return 1;
            } else {
                return 0;
            }
        }
        if(checkPhraseWordOr(words,vRange,range,dynamicVCell) == false) {
            return 1;
        }
    }
    return 0;
}

bool WordLimit::checkPhraseWordShuffle(const MeanWords& words,const VRANGE& vRange,RANGE& range,const VCell& dynamicVCell) const
{
    std::vector<RANGE> vCacheRange;

    for(size_t i=0; i<vRange.size(); i++) {
        MatchingMatrix matrix = words.generateMatchingMatrixFromWord(vRange[i],(m_vcell->getFlag() & BASIC_WORD_CELL));
        if( m_vcell->phraseShuffleMatch(matrix,dynamicVCell) == true ) {
            range = matrix.getMatchedRange();
            if(range.isLegal()) {
                range.adjustOffset(vRange[i].start);
                vCacheRange.push_back(range);
            }
        }
    }

    if(vCacheRange.empty()) return false;

    for(size_t j=0; j<vCacheRange.size(); j++) {
        if(range < vCacheRange[j]) {
            range = vCacheRange[j];
        }
    }

    return true;
}

bool WordLimit::checkPhraseWordOr(const MeanWords& words,const VRANGE& vRange,RANGE& range,const VCell& dynamicVCell) const
{
    for(size_t i=0; i<vRange.size(); i++) {
        MatchingMatrix matrix = words.generateMatchingMatrixFromWord(vRange[i],(m_vcell->getFlag() & BASIC_WORD_CELL));
        if( m_vcell->phraseMatch(matrix,dynamicVCell) == true ) {
            range = matrix.getMatchedRange();
            if(range.isLegal()) {
                range.adjustOffset(vRange[i].start);
                return true;
            }
        }
    }
    return false;
}

}//namespace sae
