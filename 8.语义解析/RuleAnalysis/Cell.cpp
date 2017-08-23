#include "Cell.hpp"

namespace sae
{

Cell::Cell()
{
    m_flags = NORMAL_CELL;
}

void Cell::copyCell(const Cell* cell)
{
    size_t i;

    m_flags |= ( cell->getFlags() & CELL_TYPE_ERASER );

    const SVector<bool>& otherTags = cell->getNegativeTags();
    for(i=0; i<otherTags.size(); i++) {
        this->negativeTags.push_back(otherTags[i]);
    }

    const SVector<int>& otherContent = cell->getContent();
    for(i=0; i<otherContent.size(); i++) {
        this->content.push_back(otherContent[i]);
    }
}

bool Cell::parseReferKey(std::string& s, const bool& negativeTag, ErrorData& errData)
{
    ERROR_TRACE( keyNum.parse(s,errData) )

    m_flags &= ~NORMAL_CELL;
    m_flags |= REFER_KEY_CELL;
    negativeTags.push_back(negativeTag);

    return true;
}




bool Cell::isAllNegative() const
{
    for(size_t i=0; i<negativeTags.size(); i++) {
        if(!negativeTags[i]) {
            return false;
        }
    }
    return true;
}

bool Cell::hasNegativeTag() const
{
    for(size_t i=0; i<negativeTags.size(); i++) {
        if(negativeTags[i]) {
            return true;
        }
    }
    return false;
}


Cell* Cell::makeNewCellByDynamicContent(const SVector<int>& content) const
{
    Cell* cell = new Cell;

    cell->copyCell(this);
    cell->content = content;
    const bool negativeTag = negativeTags[0];
    cell->negativeTags.clear();
    for(size_t i=0; i<content.size(); i++) {
        cell->negativeTags.push_back(negativeTag);
    }

    return cell;
}

bool Cell::matchAndTypeContentAtSinglePosition(const MatchingMatrix& matrix, const int& pos) const
{
    size_t i,j;

    for(i=0; i<content.size(); i++) {
        if( !(matrix[pos].find(content[i]) ^ negativeTags[i]) ) {
            return false;
        }
    }

    if( !vNegativeGroupContent.empty() ) {
        for(i=0; i<vNegativeGroupContent.size(); i++) {
            for(j=0; j<vNegativeGroupContent[i].size(); j++) {
                if( !matrix[pos].find(vNegativeGroupContent[i][j]) ) {
                    return true;
                }
            }
            return false;
        }
    }
    return true;
}

bool Cell::matchAndTypeContent(MatchingMatrix& matrix) const
{
    const int& pos = matrix.getPositionNeedToMatch();

    if( matchAndTypeContentAtSinglePosition(matrix,pos) ) {
        return matrix.movePosition(1);
    } else {
        return false;
    }
}

bool Cell::matchForwardContinuousTypeContent(MatchingMatrix& matrix) const
{
    int pos = matrix.getPositionNeedToMatch();

    if(this->content.size() > matrix.size() - pos) return false;

    for(size_t i=0; i<this->content.size(); i++) {
        if( !(matrix[pos++].find(this->content[i]) ^ negativeTags[i]) ) {
            return false;
        }
    }

    return matrix.movePosition(this->content.size());
}

bool Cell::matchBackwardContinuousTypeContent(MatchingMatrix& matrix) const
{
    int pos = matrix.getPositionNeedToMatch();

    if(this->content.size() > pos + 1) return false;

    for(int i=(int)this->content.size()-1; i>-1; i--) {
        if( !(matrix[pos--].find(this->content[i]) ^ negativeTags[i]) ) {
            return false;
        }
    }

    return matrix.movePosition(this->content.size());
}

bool Cell::matchContinuousTypeContent(MatchingMatrix& matrix) const
{
    return matrix.isBackWardSearch() ? matchBackwardContinuousTypeContent(matrix) : matchForwardContinuousTypeContent(matrix);
}


std::vector<std::string> Cell::divideCell(const std::string& s, ErrorData& errData)
{
    std::string sCache;
    std::vector<std::string> v;
    int inBracket0 = 0;

    for(size_t i=0; i<s.length(); i++) {
        if(s[i] == '&') {
            if(Utility::isTranslatedChar(s,i) == false && inBracket0 == 0) {
                v.push_back(sCache);
                sCache = "";
                if(m_flags & CONTINUOUS_RELATION) {
                    errData.m_errMsg = Message::msg068();
                    errData.errorTrace(__FILE__,__LINE__-1);
                    v.clear();
                    return v;
                } else {
                    m_flags |= AND_RELATION;
                }
                continue;
            }
        } else if(s[i] == ';') {
            if(Utility::isTranslatedChar(s,i) == false && inBracket0 == 0) {
                v.push_back(sCache);
                sCache = "";
                if(m_flags & AND_RELATION) {
                    errData.m_errMsg = Message::msg068();
                    errData.errorTrace(__FILE__,__LINE__-1);
                    v.clear();
                    return v;
                } else {
                    m_flags |= CONTINUOUS_RELATION;
                }
                continue;
            }
        } else if(s[i] == '(') {
            if(Utility::isTranslatedChar(s,i) == false) {
                inBracket0++;
            }
        } else if(s[i] == ')') {
            if(Utility::isTranslatedChar(s,i) == false) {
                inBracket0--;
            }
        }
        sCache += s[i];
    }

    if(!sCache.empty()) v.push_back(sCache);

    return v;
}

bool Cell::parseNegative(std::string& s, bool& negativeTag, ErrorData& errData)
{
    if(s[0] == '!') {
        s = s.substr(1);
        negativeTag = true;
    }
    if(s.empty()) {
        ERROR_MESSAGE( "Met an empty negative string." )
    }

    return true;
}

void Cell::dump(std::ostream& ofs) const
{
    if(m_flags & NORMAL_CELL) {
        for(size_t i=0; i<content.size(); i++) {
            if(i != 0) ofs<<(m_flags & AND_RELATION ? "&" : ";");
            if(negativeTags[i]) ofs<<"!";
            ofs<<content[i];
        }
        if(!vNegativeGroupContent.empty()) {
            if(!content.empty()) ofs<<"&";
            for(size_t j=0; j<vNegativeGroupContent.size(); j++) {
                if(j != 0) ofs<<"&";
                ofs<<"!(";
                for(size_t k=0; k<vNegativeGroupContent[j].size(); k++) {
                    if(k != 0) ofs<<"&";
                    ofs<<vNegativeGroupContent[j][k];
                }
                ofs<<")";
            }
        }
    } else {
        if(negativeTags[0]) ofs<<"!";
        keyNum.dump(ofs);
    }
}

bool Cell::isBasicWordCell() const
{
    return (m_flags & BASIC_WORD_CELL) ? true : false;
}

}//namespace sae
