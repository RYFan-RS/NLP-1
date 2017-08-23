#ifndef __SAE__KEY_PART_H__
#define __SAE__KEY_PART_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "DataManager.h"
#include "SearchMode.h"
#include "Position.h"
#include "ScriptLimit.h"
#include "WordLimit.h"
#include "TagLimit.h"

namespace sae
{

class KeyCell
{

private:
    bool m_isOmissibleKey;
    SearchMode m_searchMode;
    Position m_position;
    ScriptLimit m_scriptLimit;
    WordLimit m_wordLimit;
    TagLimit m_tagLimit;

    bool checkKeyElementsNumber(const std::string& sKey, const int& keyIndex, ErrorData& errData) const;
    bool getSearchModeFromString(std::string& sKeyIn, std::string& sSearchMode, const int& keyIndex, ErrorData& errData) const;
    bool getPositionFromString(std::string& sKey, std::string& sPosition, const int& keyIndex, ErrorData& errData) const;
    bool getScriptLimitFromString(std::string& sKey, std::string& sCharLimit,const int& keyIndex, ErrorData& errData) const;
    bool getWordLimitFromString(std::string& sKey, std::string& sWordLimit,const int& keyIndex, ErrorData& errData) const;
    bool getTagLimitFromString(std::string& sKey, std::string& sTagLimit,const int& keyIndex, ErrorData& errData) const;
    bool checkNegativeInEntryKey();
    bool hasMeaninglessCombination(ErrorData& errData) const;
    bool isPhraseSearchModeAndSingleSearchPositionCombined() const;
    bool isContinuousSearchMethodAndSingleSearchPositionCombined() const;
    bool isReferKeyCellLegal(const int& currentKeyIndex, ErrorData& errData) const;
    bool isContinuousConditionAndSingleSearchPositionCombined(ErrorData& errData) const;

public:
    bool addUsrDic(const std::map<int, std::vector<int> >& mUsrWords){
        return m_wordLimit.addUsrDic(mUsrWords);
    }
    void dump(const DataManager& dataManager, std::ostream& ofs) const;
    KeyCell() {
        m_isOmissibleKey = false;
    }

    bool readKeyString(const std::string& sKey, const int& keyIndex, DataManager* pDataManager, ErrorData& errData);

    void setOmissible() {
        m_isOmissibleKey = true;
    }
    const bool isOmissibleKey() const {
        return m_isOmissibleKey;
    }

    const SearchMode& getSearchMode() const {
        return m_searchMode;
    }
    const Position& getPosition() const {
        return m_position;
    }
    const ScriptLimit& getScriptLimit() const {
        return m_scriptLimit;
    }
    WordLimit& getWordLimit() {
        return m_wordLimit;
    }
    const WordLimit& getConstWordLimit() const {
        return m_wordLimit;
    }
    const TagLimit& getTagLimit() const {
        return m_tagLimit;
    }

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.simple(m_isOmissibleKey);
        stream.content(m_searchMode);
        stream.content(m_position);
        stream.content(m_scriptLimit);
        stream.content(m_wordLimit);
        stream.content(m_tagLimit);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.simple(m_isOmissibleKey);
        stream.content(m_searchMode);
        stream.content(m_position);
        stream.content(m_scriptLimit);
        stream.content(m_wordLimit);
        stream.content(m_tagLimit);
    }
};

class KeyPart
{

private:
    std::vector<KeyCell> m_keys;

public:
    void dump(const DataManager& dataManager, std::ostream& ofs = std::cout) const;
    std::vector<KeyCell>& getKeys() {
        return m_keys;
    }
    const std::vector<KeyCell>& getConstKeys() const {
        return m_keys;
    }
    void addKey(const KeyCell& cell) {
        m_keys.push_back(cell);
    }
    bool checkReferOfOmissibleKeyInKeys(ErrorData& errData) const;
    std::set<int> getOmissibleKeySet() const;
    bool checkPositionsOutsideBounds(ErrorData& errData) const;

    bool addUsrDic(const std::map<int, std::vector<int> >& mUsrWords){
        const std::vector<KeyCell>::iterator last = m_keys.end();
        std::vector<KeyCell>::iterator itr = m_keys.begin();
        for(; last != itr; ++itr){
            itr->addUsrDic(mUsrWords);
        }

        return true;
    }

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        //m_keys
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            KeyCell cell;
            stream.content(cell);
            m_keys.push_back(cell);
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        //m_keys
        int i,size = m_keys.size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
            stream.content(m_keys[i]);
        }
        format.output_end_array();
    }
};

}//namespace sae

#endif //__SAE__KEY_PART_H__
