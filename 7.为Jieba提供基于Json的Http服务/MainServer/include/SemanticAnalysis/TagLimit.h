#ifndef __SAE__FLAG_LIMIT_H__
#define __SAE__FLAG_LIMIT_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "DataManager.h"
#include "Range.h"
#include "MeanWord.h"
#include "VCell.hpp"

namespace sae
{

class TagLimit
{
private:
    VCell* m_vcell;
    int mc_VCellIndex;
public:
    TagLimit();
    void dump(const DataManager& dataManager, std::ostream& ofs = std::cout) const;
    const VCell* getVCellPointer() const {
        return m_vcell;
    }
    size_t size() const {
        return m_vcell == NULL ? 0 : m_vcell->size();
    }
    void push_back(Cell* cell) {
        m_vcell->push_back(cell);
    }
    bool empty() const {
        return m_vcell == NULL ? true : m_vcell->empty();
    }
    const Cell* operator [] (const int& index) const {
        return (*m_vcell)[index];
    }

    bool parse(const std::string& sTagLimit, DataManager* pDataManger, ErrorData& errData);

    int checkTag(const MeanWords& words,const VRANGE& vRange, RANGE& range) const;
    int checkLimitTag(const MeanWords& words,const VRANGE& vRange) const;
    int checkPhraseTag(const MeanWords& words,const VRANGE& vRange,RANGE& range) const;
    bool checkPhraseTagOr(const MeanWords& words,const VRANGE& vRange,RANGE& range) const;
    bool checkPhraseTagShuffle(const MeanWords& words,const VRANGE& vRange,RANGE& range) const;

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.pointer(m_vcell);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.pointer(m_vcell);
    }
};

}//namespace sae

#endif //__SAE__FLAG_LIMIT_H__
