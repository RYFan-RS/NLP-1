#ifndef __SAE__ANALYZE_ENGINE_H__
#define __SAE__ANALYZE_ENGINE_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "DequeWord.h"
#include "DataStorage.h"
#include "MeanWord.h"
#include "MeanResult.h"
#include "Config.h"
#include "Range.h"
#include "Serialization.hpp"

namespace sae
{

enum ENTRY_TYPE {
    WORD_ENTRY,
    POS_ENTRY,
    TAG_ENTRY
};

class AnalyzeEngine : public Serialization
{
private:
    DataStorage  m_data;

public:
    bool fastSerialize(const std::string& fileName);
    int init(const std::string& fileName);
    int loadUsrDic(const std::string& fileName, const std::map<std::string, std::set<std::string> >* pWords);
    DataStorage& getExecutableData() {
        return m_data;
    }
    const DataStorage& getConstExecutableData() const {
        return m_data;
    }
    void setExecutableData(const DataStorage& data) {
        m_data = data;
    }

    VMeanResult analyzePhase(const std::string& input, MeanWords& words, const ExecutePhase& phase) const;

private:
//---for matching rules--------------------------------------------------------------------------------------------------
    //--rules with the word entries
    void findRulesWithWordAndAnalyze(const MeanWords& words, const int& wordIndex, const int& phaseStart, const int& phaseEnd, VMeanResult& vPhaseMeanResult) const;
    void findRulesWithBasicWordAndAnalyze(const MeanWords& words, const int& wordIndex, const int& phaseStart, const int& phaseEnd, VMeanResult& vPhaseMeanResult) const;
    int analyzeRuleWithWord(const MeanWords& words, const int& wordIndex, const NormalRule& rule, MeanResult& result) const;

    //--rules with the tag entries
    void findRulesWithTagAndAnalyze(const MeanWords& words, int& wordIndex, const int& phaseStart, const int& phaseEnd, VMeanResult& vPhaseMeanResult) const;
    int analyzeRuleWithTag(const MeanWords& words, int& wordIndex, const NormalRule& rule, MeanResult& result) const;

    //--for checking the rules
    bool checkEntryKey(const KeyCell& key, const int& wordIndex, const MeanWords& words, RANGE& range, const ENTRY_TYPE& entryType) const;
    bool checkNormalKeys(const MeanWords& words, const std::vector<KeyCell>& keys, const int& wordIndex, VRANGE& vRange, RANGE& range, MeanResult& result) const;
    int checkEntryPosition(const MeanWords& words, const int& wordIndex, const Position& position) const;
    bool checkEntryPositionAtTail(const Position& position, const RANGE& range, const int& tail) const;
    int getWordRangesFromPosition(const MeanWords& words,const int& wordIndex, const Position& position, const MeanResult& result, VRANGE& vRange) const;

//---for executing tag rules---------------------------------------------------------------------------------------------
    int doTagRule(VDequeWord& dWords, MeanResult& result) const;
    bool applyTagsToWord(std::vector<int>& wordTags, const Cell* tagRuleTagSet) const;
    bool applyTagsToWord(std::vector<int>& wordTags, const Cell* tagRuleTagSet, std::vector<int>& deletedTags) const;

private:
    void changeMeanWordsToDequeWords(const MeanWords& words, VDequeWord& dWords, const int& wordSize) const;
    bool intersection(int& a1,int& a2, const int& b1, const int& b2) const;
    void generateParamMatrix(std::vector<std::vector<std::string> >& vvIn) const;
    template<typename T>
    int findInVec(const std::vector<T>& v, const T& n) const {
        for(size_t i=0; i<v.size(); i++) {
            if(v[i] == n) return i;
        }
        return -1;
    }

public:
    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.content(m_data);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.content(m_data);
    }
};

}//namespace sae

#endif //__SAE__ANALYZE_ENGINE_H__
