#include "DequeWord.h"

namespace sae
{

void DequeWord::dump(const DataManager& dataManager,std::ostream& ofs) const
{
    for(size_t i=0; i<this->size(); i++) {
        if(i!=0) ofs<<" ";
        (*this)[i].dump(dataManager,ofs);
    }
}

bool VDequeWord::remove(const RANGE& range)
{
    for(int i=range.start; i<=range.end; i++) {
        (*this)[i].clear();
    }

    return true;
}

MeanWords VDequeWord::getWordsInRange(const RANGE& range) const
{
    MeanWords words;

    for(int i=range.start; i<=range.end; i++) {
        for(size_t j=0; j<(*this)[i].size(); j++) {
            words.push_back((*this)[i][j]);
        }
    }

    return words;
}

MatchingMatrix VDequeWord::generateMatchingMatrixFromWord(const RANGE& range) const
{
    MatchingMatrix matrix;
    SVector<int> v;

    v.push_back(0);
    for(size_t i=0; i<this->size(); i++) {
        for(size_t j=0; j<(*this)[i].size(); j++) {
            v[0] = (*this)[i][j].getWord();
            matrix.push_back(v);
        }
    }

    return matrix;
}

void VDequeWord::addSentenceTag(const int& tag)
{
    m_tags.push_back(tag);
}

void VDequeWord::deleteSentenceTag(const int& tag)
{
    m_tags.remove(tag);
}

bool VDequeWord::hold(const RANGE& range)
{
    for(int i=range.start; i<=range.end; i++) {
        for(size_t j=0; j<(*this)[i].size(); j++) {
            m_holdWords.push_back((*this)[i][j]);
        }
        (*this)[i].clear();
    }

    return true;
}



}//namespace sae
