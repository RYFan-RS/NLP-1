#include "Range.h"

namespace sae
{

const RANGE RANGE::Error;

RANGE::RANGE()
{
    start = -1;
    end = -1;
    backwardSearch = false;
    continuousSearch = false;
}

RANGE::RANGE(const int& start, const int& end)
{
    this->start = start;
    this->end = end;
    backwardSearch = false;
    continuousSearch = false;
}

RANGE::RANGE(const int& start, const int& end, const bool& backwardSearch, const bool& continuousSearch)
{
    this->start = start;
    this->end = end;
    this->backwardSearch = backwardSearch;
    this->continuousSearch = continuousSearch;
}

void RANGE::clear()
{
    start = -1;
    end = -1;
}

void RANGE::dump(std::ostream& ofs) const
{
    ofs<<"[";
    if(!backwardSearch && continuousSearch) {
        ofs<<":";
    }
    ofs<<start;
    if(backwardSearch) {
        ofs<<"<-";
    } else {
        ofs<<"->";
    }
    ofs<<end;
    if(backwardSearch && continuousSearch) {
        ofs<<":";
    }
    ofs<<"]";
}

VRANGE::VRANGE()
{
}

void VRANGE::clear()
{
    erase(begin(),end());
}

void VRANGE::dump(std::ostream& ofs) const
{
    for(size_t i=0; i<size(); i++) {
        (*this)[i].dump(ofs);
    }
}

RANGE VRANGE::getWordRangeFromKeyNum(const KeyNum& keyNum) const
{
    RANGE range;

    if(keyNum == KeyNum::Error) {
        return RANGE::Error;
    }

    if(keyNum.m_subKeyNum < 0) {
        if(keyNum.getKeyNum() >= (int)(*this).size() || keyNum.getKeyNum() < 0) {
            return RANGE::Error;
        } else {
            range = (*this)[keyNum.getKeyNum()];
        }
    } else {
        if(keyNum.getKeyNum() >= (int)(*this).size() || keyNum.getKeyNum() < 0) {
            return RANGE::Error;
        } else {
            if(keyNum.m_subKeyNum >= TAIL_RANGE) { //T type
                range.start = (*this)[keyNum.getKeyNum()].end - keyNum.getSubKeyNum() + TAIL;
            } else {
                range.start = (*this)[keyNum.getKeyNum()].start + keyNum.getSubKeyNum();
            }

            //out of range
            if(range.start > (*this)[keyNum.getKeyNum()].end) {
                range.start = (*this)[keyNum.getKeyNum()].end;
            }
            range.end = range.start;
        }
    }

    return range;
}

RANGE VRANGE::getWordRangeFromKeyRange(const KeyRange& keyRange) const
{
    RANGE startKeyRange = getWordRangeFromKeyNum(keyRange.getStartKeyNum());
    if(startKeyRange == RANGE::Error) {
        return RANGE::Error;
    }

    RANGE endKeyRange = getWordRangeFromKeyNum(keyRange.getEndKeyNum());
    if(endKeyRange == RANGE::Error) {
        return startKeyRange;
    }

    return caculateUnionWordRange(startKeyRange,endKeyRange);
}

RANGE VRANGE::caculateUnionWordRange(const RANGE& r1, const RANGE& r2) const
{
    RANGE range;

    range.start = TheMinOf(r1.start,r2.start);
    range.end  = TheMaxOf(r1.end,r2.end);

    return range;
}

bool VRANGE::isConflictWithWordRange(const RANGE& range) const
{
    for(size_t i=0; i<size(); i++) {
        if(range.start > (*this)[i].start && range.start <= (*this)[i].end) {
            return true;
        } else if(range.end >= (*this)[i].start && range.end < (*this)[i].end) {
            return true;
        }
    }

    return false;
}

void RANGE::adjustOffset(const int& offset)
{
    this->start += offset;
    this->end += offset;
}

bool RANGE::isNull() const
{
    return start==-1 || end == -1 ? true : false;
}

bool RANGE::isLegal() const
{
    return !isNull() && start <= end ? true : false;
}

void RANGE::setBackwardSearchFlag(const bool& flag)
{
    backwardSearch = flag;
}

void RANGE::setContinuousSearchFlag(const bool& flag)
{
    continuousSearch = flag;
}

void VRANGE::setContinuousSearchFlag(const bool& flag)
{
    for(size_t i=0; i<this->size(); i++) {
        (*this)[i].setContinuousSearchFlag(flag);
    }
}

}//namespace sae
