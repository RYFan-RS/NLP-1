#ifndef __SAE__DEQUE_WORD_H__
#define __SAE__DEQUE_WORD_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <deque>
#include <vector>
#include "MeanWord.h"
#include "Range.h"

namespace sae
{

class DequeWord : public std::deque<MeanWord>
{
public:
    void dump(const DataManager& dataManager,std::ostream& ofs = std::cout) const;
};

class VDequeWord : public std::vector<DequeWord>
{
public:
    SVector<int> m_tags;
    std::vector<MeanWord> m_holdWords;
public:
    MeanWords getWordsInRange(const RANGE& range) const;
    MatchingMatrix generateMatchingMatrixFromWord(const RANGE& range) const;

    const SVector<int>& getTags() const {
        return m_tags;
    }
    void setTags(const SVector<int>& tags) {
        m_tags = tags;
    }

    const std::vector<MeanWord>& getHoldWords() const {
        return m_holdWords;
    }
    void setHoldWords(const std::vector<MeanWord>& words) {
        m_holdWords = words;
    }


    bool remove(const RANGE& range);
    bool hold(const RANGE& range);
    void addSentenceTag(const int& tag);
    void deleteSentenceTag(const int& tag);
};

}//namespace sae

#endif //__SAE__DEQUE_WORD_H__
