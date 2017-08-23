#ifndef __SAE__RANGE_H__
#define __SAE__RANGE_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <fstream>
#include <vector>
#include "KeyRange.h"

namespace sae
{

class RANGE
{
private:
    bool backwardSearch;
    bool continuousSearch;
public:
    int start;
    int end;
    static const RANGE Error;
public:
    RANGE();
    RANGE(const int& start, const int& end);
    RANGE(const int& start, const int& end, const bool& backwardSearch, const bool& continuousSearch);
    void clear();

    bool operator == (const RANGE& r) const {
        return start==r.start && end==r.end ? true : false;
    }

    bool operator != (const RANGE& r) const {
        return start!=r.start || end!=r.end ? true : false;
    }

    void operator = (const RANGE& r) {
        start = r.start;
        end = r.end;
    }

    bool operator > (const RANGE& r) const {
        return length() > r.length() ? true : false;
    }

    bool operator < (const RANGE& r) const {
        return length() < r.length() ? true : false;
    }

    int length() const {
        return isLegal() ? (end-start+1) : 0;
    }

    bool isRange() const {
        return start == end ? false : true;
    }

    void dump(std::ostream& ofs = std::cout) const;
    friend std::ostream& operator << (std::ostream& ofs, const RANGE& range) {
        range.dump(ofs);
        return ofs;
    }

    void adjustOffset(const int& offset);

    bool isNull() const;
    bool isLegal() const;
    void setBackwardSearchFlag(const bool& flag);
    const bool& isBackwardSearch() const {
        return backwardSearch;
    }
    void setContinuousSearchFlag(const bool& flag);
    const bool& isContinuousSearch() const {
        return continuousSearch;
    }
};

class VRANGE : public std::vector<RANGE>
{
public:
    VRANGE();
    void clear();
    RANGE getWordRangeFromKeyNum(const KeyNum& keyNum) const;
    RANGE getWordRangeFromKeyRange(const KeyRange& keyRange) const;
    RANGE caculateUnionWordRange(const RANGE& r1, const RANGE& r2) const;
    bool isConflictWithWordRange(const RANGE& range) const;
    void setContinuousSearchFlag(const bool& flag);
    void dump(std::ostream& ofs = std::cout) const;
};

}//namespace sae

#endif //__SAE__RANGE_H__
