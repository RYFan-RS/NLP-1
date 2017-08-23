#ifndef __SAE__MATCHING_MATRIX_H__
#define __SAE__MATCHING_MATRIX_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "SerializableVector.hpp"
#include "Range.h"

namespace sae
{

class MatchingMatrix : public SVector<SVector<int> >
{
private:
    int firstNeedToMatchPosition;
    int firstMatchedPosition;
    int positionNeedToMatch;
    bool backwardSearch;

private:
    bool moveFirstMatchedPosition();
    bool isContinuousSearch() const;
public:
    MatchingMatrix();
    const int& getPositionNeedToMatch() const {
        return positionNeedToMatch;
    }
    void setPositionNeedToMatch(const int& position);
    const int& getFirstMatchedPosition() const {
        return firstMatchedPosition;
    }
    void setFirstMatchedPosition(const int& position);
    void setFirstNeedToMatchPosition();
    void setToBackWardSearch();
    const bool& isBackWardSearch() const {
        return backwardSearch;
    }

    bool movePositionNeedToMatch(const int& distance);
    bool movePosition(const int& distance);
    bool hasMatchedSomething() const;
    bool isCheckCompleted() const;
    RANGE getMatchedRange() const;
    bool matchedAll() const;

//unused:
    void resetState();

    void dump(std::ostream& ofs = std::cout) const;

    template<class P>
    void dump(std::ostream& ofs = std::cout, P *p = NULL) const {
        ofs<<"MatchingMatrix state:"<<std::endl;
        ofs<<"\tfirstMatchedPosition = "<<firstMatchedPosition<<std::endl;
        ofs<<"\tpositionNeedToMatch = "<<positionNeedToMatch<<std::endl;
        ofs<<"\tbackwardSearch = "<<(backwardSearch==true ? "true" : "false")<<std::endl;
        for(size_t i=0; i<this->size(); i++) {
            ofs<<"\tmm["<<i<<"]: ";
            for(size_t j=0; j<(*this)[i].size(); j++) {
                if(j != 0) ofs<<" ";
                if(p == NULL) {
                    ofs<<(*this)[i][j];
                } else {
                    ofs<<p->find((*this)[i][j]);
                }
            }
            ofs<<std::endl;
        }
    }

};

}//namespace sae

#endif //__SAE__MATCHING_MATRIX_H__
