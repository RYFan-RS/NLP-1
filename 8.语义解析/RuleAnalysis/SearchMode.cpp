#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "Utility.h"
#include "SearchMode.h"

namespace sae
{

bool SearchMode::readSearchModeString(const std::string& sSearchMode, const int& keyIndex, ErrorData& errData)
{
    std::vector<std::string> vSearchModeCell = sae::Utility::divide(sSearchMode,"&");

    for(size_t i=0; i<vSearchModeCell.size(); i++) {
        SearchModeCell cell;
        size_t length = vSearchModeCell[i].length();

        if(length == 2 || length == 3) {
            if(vSearchModeCell[i][0] == 'C') {
                cell.continuousTag = true;
                vSearchModeCell[i] = vSearchModeCell[i].substr(1);
                length--;
            }
        }

        if(length == 1) {
            if(vSearchModeCell[i] == "W") {
                cell.m_searchType = WORD_TYPE;
            }else if(vSearchModeCell[i] == "T") {
                cell.m_searchType = TAG_TYPE;
            } else {
                errData.m_errMsg = Message::msg094(vSearchModeCell[i],keyIndex);
                errData.errorTrace(__FILE__,__LINE__-1);
                return false;
            }
        } else if(length == 2) {
            if(vSearchModeCell[i][0] == 'L') {
                cell.m_limitMode = true;
            } else if(vSearchModeCell[i][0] == 'P') {
                cell.m_phraseMode = true;
            } else {
                errData.m_errMsg = Message::msg094(vSearchModeCell[i],keyIndex);
                errData.errorTrace(__FILE__,__LINE__-1);
                return false;
            }
            if(vSearchModeCell[i][1] == 'W') {
                cell.m_searchType = WORD_TYPE;
            }else if(vSearchModeCell[i][1] == 'T') {
                cell.m_searchType = TAG_TYPE;
            } else {
                errData.m_errMsg = Message::msg094(vSearchModeCell[i],keyIndex);
                errData.errorTrace(__FILE__,__LINE__-1);
                return false;
            }
        } else if(length == 0) {
            errData.m_errMsg = Message::msg095(keyIndex);
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        } else {
            errData.m_errMsg = Message::msg094(vSearchModeCell[i],keyIndex);
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }

        m_cells.push_back(cell);
    }

    if( hasMeaninglessCombination() ) {
        errData.m_errMsg = Message::msg096(keyIndex);
        errData.errorTrace(__FILE__,__LINE__-1);
        return false;
    }

    return true;
}

bool SearchMode::hasWordSearchType() const
{
    for(size_t i=0; i<m_cells.size(); i++) {
        if(m_cells[i].m_searchType == WORD_TYPE) {
            return true;
        }
    }
    return false;
}

bool SearchMode::hasTagSearchType() const
{
    for(size_t i=0; i<m_cells.size(); i++) {
        if(m_cells[i].m_searchType == TAG_TYPE) {
            return true;
        }
    }
    return false;
}

bool SearchMode::hasNormalMode() const
{
    for(size_t i=0; i<m_cells.size(); i++) {
        if(m_cells[i].m_phraseMode == false && m_cells[i].m_limitMode == false) {
            return true;
        }
    }
    return false;
}

bool SearchMode::hasPhraseMode() const
{
    for(size_t i=0; i<m_cells.size(); i++) {
        if(m_cells[i].m_phraseMode == true) {
            return true;
        }
    }
    return false;
}

bool SearchMode::hasLimitMode() const
{
    for(size_t i=0; i<m_cells.size(); i++) {
        if(m_cells[i].m_limitMode == true) {
            return true;
        }
    }
    return false;
}

bool SearchMode::hasContinuousMode() const
{
    for(size_t i=0; i<m_cells.size(); i++) {
        if(m_cells[i].continuousTag == true) {
            return true;
        }
    }
    return false;
}

void SearchModeCell::dump(std::ostream& ofs) const
{
    if(continuousTag == true) {
        ofs<<"C";
    }
    if(m_limitMode == true) {
        ofs<<"L";
    }
    if(m_phraseMode == true) {
        ofs<<"P";
    }
    if(m_searchType == WORD_TYPE) {
        ofs<<"W";
    }else if(m_searchType == TAG_TYPE) {
        ofs<<"T";
    } else {
        ofs<<"#ERROR";
    }
}

void SearchMode::dump(std::ostream& ofs) const
{
    for(size_t i=0; i<m_cells.size(); i++) {
        if(i != 0) {
            ofs<<"&";
        }
        m_cells[i].dump(ofs);
    }
}

bool SearchMode::hasMeaninglessCombination() const
{
    return hasPhraseMode() && hasNormalMode() ? true : false;
}

}//namespace sae
