#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "TagLimit.h"
#include "CommonParser.h"
#include "Utility.h"

namespace sae
{

TagLimit::TagLimit()
{
    m_vcell = NULL;
    mc_VCellIndex = -1;
}

void TagLimit::dump(const DataManager& dataManager, std::ostream& ofs) const
{
    if(m_vcell == NULL) ofs<<"*";
    else {
        for(size_t i=0; i<m_vcell->size(); i++) {
            if(i != 0) {
                if(m_vcell->getFlag() == OR_RELATION) ofs<<"/";
                else ofs<<"|";
            }
            (*m_vcell)[i]->dump(dataManager.getConstTagManager(),ofs);
        }
    }
}

bool TagLimit::parse(const std::string& sTagLimit, DataManager* pDataManger, ErrorData& errData)
{
    int index = gp_VCellCache->find(sTagLimit,TAG_DATABASE);
    if(index == -1) {
        m_vcell = new VCell;
        index = gp_VCellCache->insert(sTagLimit,m_vcell,TAG_DATABASE);
        if(m_vcell->parse(sTagLimit,pDataManger->getDefineManager(),
                          pDataManger->getTagManager(),errData) == false) {
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }
    } else {
        m_vcell = (*gp_VCellCache)[index];
    }
    mc_VCellIndex = index;

    return true;
}

int TagLimit::checkTag(const MeanWords& words,const VRANGE& vRange, RANGE& range) const
{
    for(size_t i=0; i<vRange.size(); i++) {
        MatchingMatrix matrix = words.generateMatchingMatrixFromTag(vRange[i]);
        if( m_vcell->forwardMatch(matrix) == true ) {
            range = matrix.getMatchedRange();
            if(range.isLegal()) {
                range.adjustOffset(vRange[i].start);
                return 0;
            }
        }
    }

    return 1;
}

int TagLimit::checkLimitTag(const MeanWords& words,const VRANGE& vRange) const
{
    for(size_t i=0; i<vRange.size(); i++) {
        MatchingMatrix matrix = words.generateMatchingMatrixFromTag(vRange[i]);
        if(m_vcell->allMatch(matrix) == false) {
            return 1;
        }
    }

    return 0;
}

int TagLimit::checkPhraseTag(const MeanWords& words,const VRANGE& vRange, RANGE& range) const
{
    switch(m_vcell->getFlag()) {
    case SHUFFLE_RELATION:
        if(checkPhraseTagShuffle(words,vRange,range) == false) {
            return 1;
        }
        break;
    case OR_RELATION:
    default:
        if(checkPhraseTagOr(words,vRange,range) == false) {
            return 1;
        }
    }
    return 0;
}

bool TagLimit::checkPhraseTagShuffle(const MeanWords& words,const VRANGE& vRange, RANGE& range) const
{
    std::vector<RANGE> vCacheRange;

    for(size_t i=0; i<vRange.size(); i++) {
        MatchingMatrix matrix = words.generateMatchingMatrixFromTag(vRange[i]);
        if( m_vcell->phraseShuffleMatch(matrix) == true ) {
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

bool TagLimit::checkPhraseTagOr(const MeanWords& words,const VRANGE& vRange, RANGE& range) const
{
    for(size_t i=0; i<vRange.size(); i++) {
        MatchingMatrix matrix = words.generateMatchingMatrixFromTag(vRange[i]);
        if( m_vcell->phraseMatch(matrix) == true ) {
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
