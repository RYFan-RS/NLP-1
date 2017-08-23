#include "MatchingMatrix.h"

namespace sae
{

MatchingMatrix::MatchingMatrix()
{
    firstNeedToMatchPosition=-1;
    firstMatchedPosition=-1;
    positionNeedToMatch = 0;
    backwardSearch = false;
}

void MatchingMatrix::setPositionNeedToMatch(const int& position)
{
    positionNeedToMatch = position;
}

void MatchingMatrix::setFirstMatchedPosition(const int& position)
{
    firstMatchedPosition = position;
}

void MatchingMatrix::setFirstNeedToMatchPosition()
{
    if(backwardSearch) {
        firstNeedToMatchPosition = this->size() - 1;
    } else {
        firstNeedToMatchPosition = 0;
    }
}

void MatchingMatrix::setToBackWardSearch()
{
    backwardSearch = true;
    firstMatchedPosition = this->size();
    positionNeedToMatch = this->size() - 1;
}

bool MatchingMatrix::movePositionNeedToMatch(const int& distance)
{
    if(backwardSearch) {
        positionNeedToMatch -= distance;
        return positionNeedToMatch > -1 ? true : false;
    } else {
        positionNeedToMatch += distance;
        return positionNeedToMatch < (int)this->size() ? true : false;
    }
}

bool MatchingMatrix::moveFirstMatchedPosition()
{
    if( firstNeedToMatchPosition != -1 && positionNeedToMatch != firstNeedToMatchPosition ) {
        return false;
    }
    firstMatchedPosition = positionNeedToMatch;
    return true;
}

bool MatchingMatrix::movePosition(const int& distance)
{
    if(backwardSearch) {
        if(firstMatchedPosition == (int)this->size()) {
            if( !moveFirstMatchedPosition() ) return false;
        }
    } else {
        if(firstMatchedPosition == -1) {
            if( !moveFirstMatchedPosition() ) return false;
        }
    }
    movePositionNeedToMatch(distance);
    return true;
}

bool MatchingMatrix::hasMatchedSomething() const
{
    if(backwardSearch) {
        return firstMatchedPosition < (int)this->size() ? true : false;
    } else {
        return firstMatchedPosition > -1 ? true : false;
    }
}

RANGE MatchingMatrix::getMatchedRange() const
{
    if(backwardSearch) {
        return firstMatchedPosition < (int)this->size() && positionNeedToMatch < (int)this->size() -1
               ? RANGE(positionNeedToMatch+1,firstMatchedPosition,backwardSearch,isContinuousSearch()) : RANGE::Error;
    } else {
        return firstMatchedPosition > -1 && positionNeedToMatch > 0 ? RANGE(firstMatchedPosition
                ,positionNeedToMatch-1,backwardSearch,isContinuousSearch()) : RANGE::Error;
    }
}

bool MatchingMatrix::matchedAll() const
{
    if(backwardSearch) {
        return firstMatchedPosition == (int)this->size() - 1 && positionNeedToMatch == -1 ? true : false;
    } else {
        return firstMatchedPosition == 0 && positionNeedToMatch == (int)this->size() ? true : false;
    }
}

bool MatchingMatrix::isCheckCompleted() const
{
    if(backwardSearch) {
        return positionNeedToMatch == -1 ? true : false;
    } else {
        return positionNeedToMatch == (int)this->size() ? true : false;
    }
}

void MatchingMatrix::resetState()
{
    if(backwardSearch) {
        firstMatchedPosition = this->size();
        positionNeedToMatch = this->size() - 1;
    } else {
        firstMatchedPosition = -1;
        positionNeedToMatch = 0;
    }
}

void MatchingMatrix::dump(std::ostream& ofs) const
{
    ofs<<"MatchingMatrix state:"<<std::endl;
    if(firstNeedToMatchPosition != -1) {
        ofs<<"\tfirstNeedToMatchPosition = "<<firstNeedToMatchPosition<<std::endl;
    }
    ofs<<"\tfirstMatchedPosition = "<<firstMatchedPosition<<std::endl;
    ofs<<"\tpositionNeedToMatch = "<<positionNeedToMatch<<std::endl;
    ofs<<"\tbackwardSearch = "<<(backwardSearch==true ? "true" : "false")<<std::endl;
    for(size_t i=0; i<this->size(); i++) {
        ofs<<"\tmm["<<i<<"]: ";
        for(size_t j=0; j<(*this)[i].size(); j++) {
            if(j != 0) ofs<<" ";
            ofs<<(*this)[i][j];
        }
        ofs<<std::endl;
    }
}

bool MatchingMatrix::isContinuousSearch() const
{
    return firstNeedToMatchPosition == -1 ? false : true;
}

}//namespace sae
