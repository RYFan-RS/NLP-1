#ifndef __SAE__WORD_LIMIT_H__
#define __SAE__WORD_LIMIT_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "DataManager.h"
#include "Range.h"
#include "MeanWord.h"
#include "VCell.hpp"


namespace sae
{

class WordLimit
{
private:
    VCell* m_vcell;
    int mc_VCellIndex;
public:
    WordLimit();
    void dump(const DataManager& dataManager, std::ostream& ofs = std::cout) const;
    bool isAllWordInSet(std::set<int>& setKey) const;
    // a;b/c;d;e or a/b/c
    std::vector<std::string> getSimpleCombinedTypeWords(const DataManager& dataManager) const;
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

    bool addUsrDic(const std::map<int, std::vector<int> >& mUsrWords){
        if(m_vcell){
            return m_vcell->addUsrDic(mUsrWords);
        }
        return true;
    }
    bool hasUnisolatedComma(const std::string &s) const;

    bool parse(const std::string& sWordLimit, DataManager* pDataManger, ErrorData& errData);
    void makeReferKeyInstant(const MeanWords& words, const VRANGE& keyRanges, const DataManager& dataManager, VCell& dynamicVCell) const;

    int checkWord(const MeanWords& words,const VRANGE& vRange,RANGE& range,const VCell& dynamicVCell=g_EmptyVCell) const;
    int checkLimitWord(const MeanWords& words,const VRANGE& vRange,const VCell& dynamicVCell=g_EmptyVCell) const;
    int checkPhraseWord(const MeanWords& words,const VRANGE& vRange,RANGE& range,const VCell& dynamicVCell=g_EmptyVCell) const;
    bool checkPhraseWordOr(const MeanWords& words,const VRANGE& vRange,RANGE& range,const VCell& dynamicVCell=g_EmptyVCell) const;
    bool checkPhraseWordShuffle(const MeanWords& words,const VRANGE& vRange,RANGE& range,const VCell& dynamicVCell=g_EmptyVCell) const;

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

#endif //__SAE__WORD_LIMIT_H__
