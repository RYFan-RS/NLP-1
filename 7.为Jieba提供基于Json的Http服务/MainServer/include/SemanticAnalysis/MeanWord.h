#ifndef __SAE__MEAN_WORD_H__
#define __SAE__MEAN_WORD_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#else
#include <pthread.h>
#endif //WIN32
#include "ErrorData.h"
#include "DataManager.h"
#include "MatchingMatrix.h"
#include "SerializableVector.hpp"
#include "Range.h"
#include "MeanWordsStringInterface.h"
#include "Words.h"

namespace sae
{

class MeanWord
{
public:
    bool m_isDigitalWord;
    int  m_word;
    std::string m_sWord;
    int  m_basicWord;
    std::string m_sBasicWord;
    SVector<int> m_tags;

    std::string m_cacheBasicOrg;
#ifndef WIN32
    static pthread_mutex_t m_mutex_mae_meanword_locker;
#endif //WIN32
public:
    MeanWord();
    bool operator ==(const MeanWord& otherWord) const;
    bool operator !=(const MeanWord& otherWord) const;
    const int& getWord() const {
        return m_word;
    }
    void setWord(const int& word);
    void setWord(const std::string& word, const DataManager& dataManager);
    const int& getBasicWord() const {
        return m_basicWord;
    }
    void setBasicWord(const std::string& basicWord, const DataManager& dataManager);

    const std::string& getStrWord(const DataManager& dataManager) const;
    std::string getStrBasicWord(const DataManager& dataManager) const;

    const SVector<int>& getTags() const {
        return m_tags;
    }
    void setTags(const SVector<int>& tags);
    void addTag(const int& tag) {
        m_tags.push_back(tag);
    }
    void clearTags() {
        m_tags.clear();
    }
    std::string getStrTags(const DataManager& dataManager) const;

    void dump(const DataManager& dataManager,std::ostream& ofs = std::cout) const;
    std::string dump2Str(const DataManager& dataManager) const;
};

class MeanWords : public std::vector<MeanWord>
{
private:
    SVector<int> m_tags;
    std::vector<MeanWord> m_holdWords;
    std::string	m_orignalString;
    DataManager* mp_DataManager;

public:
    MeanWords() {}
    MeanWords(const std::vector<std::string>& vWord, DataManager* pData);
	MeanWords(const aisms::Words& words, DataManager* pData);
    MeanWords(MeanWordsStringInterface& stringInterface, DataManager* pData);
    bool operator ==(const MeanWords& otherWords) const;
    bool operator !=(const MeanWords& otherWords) const;
    const SVector<int>& getTags() const {
        return m_tags;
    }
    void setTags(const SVector<int>& tags);
    void dump(std::ostream& ofs = std::cout) const;
    std::string dump2Str() const;
    std::string dumpHoldWords() const;
    std::string getStrTags() const;
    MatchingMatrix generateMatchingMatrixFromWord(const RANGE& range, const bool& isBasicWord) const;
    MatchingMatrix generateMatchingMatrixFromTag(const RANGE& range) const;
    MatchingMatrix generateMatchingMatrixFromSentenceTag() const;
    SVector<int> generateDynamicContentFromWord(const RANGE& range, const int& offset, const DataManager& dataManager) const;
    void setHoldWords(const std::vector<MeanWord>& words);
    const std::vector<MeanWord>& getHoldWords() const;
	std::map<std::string,std::string> getTagContentMap() const;
	std::multimap<std::string,std::string> getTagContentMultiMap() const;
};

}//namespace sae

#endif //__SAE__MEAN_WORD_H__
