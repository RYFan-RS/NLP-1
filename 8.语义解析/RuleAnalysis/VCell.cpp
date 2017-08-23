#include "VCell.hpp"
#include <algorithm>

namespace sae
{

const VCell g_EmptyVCell;

VCell::VCell()
{
    m_flag = 0;
}

bool VCell::parseNegative(std::string& s, bool& negativeTag, ErrorData& errData)
{
    if(s[0] == '!') {
        s = s.substr(1);
        negativeTag = true;
    }
    if(s.empty()) {
        ERROR_MESSAGE( Message::msg058() )
    }

    return true;
}

bool VCell::checkNegativeTagsAndContentLength(ErrorData& errData) const
{
    for(size_t i=0; i<this->size(); i++) {
        const size_t nTagsLen = (*this)[i]->getNegativeTags().size();
        const size_t contentLen = (*this)[i]->getContent().size();

        if( nTagsLen != contentLen ) {
            if( nTagsLen == 1 && contentLen == 0 && (*this)[i]->isReferKeyCell() ) {
                continue;
            }
            ERROR_MESSAGE( Message::msg059(nTagsLen,contentLen) )
        }
    }

    return true;
}

bool VCell::allMatch(MatchingMatrix& matrix,const VCell& dynamicVCell) const
{
    int dynamicVCellIndex = 0;

    for(int i=0; i<(int)this->size(); i++) {
        if( (*this)[i]->isReferKeyCell() ) {
            if(dynamicVCell[dynamicVCellIndex++]->match(matrix) == true) {
                if(matrix.matchedAll()) return true;
                i = -1;
                dynamicVCellIndex = 0;
            }
        } else {
            if((*this)[i]->match(matrix) == true) {
                if(matrix.matchedAll()) return true;
                i = -1;
                dynamicVCellIndex = 0;
            }
        }
    }

    return false;
}

bool VCell::forwardMatch(MatchingMatrix& matrix,const VCell& dynamicVCell) const
{
    while(1) {
        int dynamicVCellIndex = 0;

        for(size_t i=0; i<this->size(); i++) {
            if( (*this)[i]->isReferKeyCell() ) {
                if(dynamicVCell[dynamicVCellIndex++]->match(matrix) == true) {
                    return true;
                }
            } else {
                if((*this)[i]->match(matrix) == true) {
                    return true;
                }
            }
//#ifdef MEAN_DEBUG
//			matrix.dump(ssDebug);
//			acl::debug::MESSAGE(MSG_STREAM,ssDebug,acl::debug::LEVEL3);
//			ssDebug.str("");
//#endif //MEAN_DEBUG
        }
        if(!matrix.movePositionNeedToMatch(1)) break;
    }

    return false;
}

bool VCell::phraseMatch(MatchingMatrix& matrix,const VCell& dynamicVCell) const
{
    int i;

    while(1) {
        int dynamicVCellIndex = 0;

        for(i=0; i<(int)this->size(); i++) {
            if( (*this)[i]->isReferKeyCell() ) {
                if(dynamicVCell[dynamicVCellIndex++]->match(matrix)) {
                    if(matrix.isCheckCompleted()) break;
                    i = -1;
                    dynamicVCellIndex = 0;
                }
            } else {
                if((*this)[i]->match(matrix)) {
                    if(matrix.isCheckCompleted()) break;
                    i = -1;
                    dynamicVCellIndex = 0;
                }
            }
        }

        if(matrix.hasMatchedSomething()) return true;
        if(!matrix.movePositionNeedToMatch(1)) break;
    }
    return false;
}

bool VCell::addUsrDic(const std::map<int, std::vector<int> >& mUsrWords){
    const std::map<int, std::vector<int> >::const_iterator last = mUsrWords.end();
    std::map<int, std::vector<int> >::const_iterator itr = mUsrWords.begin();
    for(; last != itr; ++itr){
        std::vector<int>::iterator itrFind = std::find(m_vNameIndexs.begin(), m_vNameIndexs.end(), itr->first);
        if(itrFind == m_vNameIndexs.end()){
            continue;
        }
        const std::vector<int>& vUsrWords = itr->second;
        const std::vector<int>::const_iterator l = vUsrWords.end();
        std::vector<int>::const_iterator i = vUsrWords.begin();
        for(;l != i; ++i){
            Cell* cell = new Cell;
            cell->addUsrDic(*i);
            gp_CellCache->push_back(cell);
            mc_CellIndex.push_back(gp_CellCache->size() - 1);
            push_back(cell);
        }
    }

    return true;
}

std::vector<int> VCell::getFirstMatchedList(MatchingMatrix& matrix,const VCell& dynamicVCell) const
{
    std::vector<int> vIndex;
    int pos = -1;

    while(1) {
        int dynamicVCellIndex = 0;

        for(size_t i=0; i<this->size(); i++) {
            if( (*this)[i]->isReferKeyCell() ) {
                if(dynamicVCell[dynamicVCellIndex++]->match(matrix) == true) {
                    vIndex.push_back(i);
                    pos = matrix.getMatchedRange().start;
                    for(size_t j=i+1; j<this->size(); j++) {
                        if( (*this)[j]->matchAndTypeContentAtSinglePosition(matrix,pos) ) {
                            vIndex.push_back(j);
                        }
                    }
                    return vIndex;
                }
            } else { //not a refer key cell
                if((*this)[i]->match(matrix) == true) {
                    vIndex.push_back(i);
                    pos = matrix.getMatchedRange().start;
                    for(size_t j=i+1; j<this->size(); j++) {
                        if( (*this)[j]->matchAndTypeContentAtSinglePosition(matrix,pos) ) {
                            vIndex.push_back(j);
                        }
                    }
                    return vIndex;
                }
            }
        }
        if(!matrix.movePositionNeedToMatch(1)) break;
    }

    return vIndex;
}

bool VCell::phraseShuffleMatch(MatchingMatrix& matrix,const VCell& dynamicVCell) const
{
    size_t i;
    MatchingMatrix originalMatrix = matrix;

    std::vector<int> vIndex = getFirstMatchedList(matrix,dynamicVCell);
    if(vIndex.empty()) return false;

    for(i=0; i<vIndex.size(); i++) {
        VCell cacheVCell;
        cacheVCell.push_back((*this)[vIndex[i]]);
        MatchingMatrix cacheMatrix = originalMatrix;
        if( cacheVCell.phraseMatch(cacheMatrix) ) {
            if(matrix.getMatchedRange() < cacheMatrix.getMatchedRange()) {
                matrix = cacheMatrix;
            }
        }
    }

    return matrix.hasMatchedSomething() ? true : false;
}

bool VCell::isAllNegative() const
{
    for(size_t i=0; i<this->size(); i++) {
        if((*this)[i]->isAllNegative() == false) {
            return false;
        }
    }
    return true;
}

bool VCell::isSimpleCellString(const std::string& s) const
{
    size_t pos;

    pos = s.find("&");
    if( pos != std::string::npos && !Utility::isTranslatedChar(s,pos) ) {
        return false;
    }

    pos = s.find(";");
    if( pos != std::string::npos && !Utility::isTranslatedChar(s,pos) ) {
        return false;
    }


    if( (s.find("@&)") != std::string::npos) || (s.find("@;)") != std::string::npos) ) {
        return false;
    }

    return true;
}

bool VCell::isReferKeyCellLegal(const int& currentKeyIndex) const
{
    for(size_t i=0; i<this->size(); i++) {
        if( (*this)[i]->isReferKeyCell() && (*this)[i]->getKeyNum().getKeyNum() >= currentKeyIndex) {
            return false;
        }
    }
    return true;
}

bool VCell::hasContinuousCell() const
{
    for(size_t i=0; i<this->size(); i++) {
        if( !(*this)[i]->isReferKeyCell() && !(*this)[i]->isAndRelation()) {
            return true;
        }
    }
    return false;
}

void VCell::freeCellMemories()
{
    for(size_t i=0; i<this->size(); i++) {
        delete (*this)[i];
    }
    this->clear();
}

void VCell::parseBasicWordMark()
{
    if( (*this)[0]->getFlags() & BASIC_WORD_CELL ) {
        m_flag |= BASIC_WORD_CELL;
    }
}

}//namespace sae
