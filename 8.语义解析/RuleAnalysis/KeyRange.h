#ifndef __SAE_KEY_RANGE_H__
#define __SAE_KEY_RANGE_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "ErrorData.h"
#include "Utility.h"
#include "TypeDefine.h"

namespace sae
{

class KeyNum
{
public:
    int m_keyNum;
    int m_subKeyNum;
    static const KeyNum Error;
public:
    KeyNum() {
        m_keyNum=-1;
        m_subKeyNum=-1;
    }
    const int& getKeyNum() const {
        return m_keyNum;
    }
    const int& getSubKeyNum() const {
        return m_subKeyNum;
    }
    bool isWholeSentence() const;
    bool parse(const std::string& sKey, ErrorData& errData);
    bool isKeyNumOverMaxKeyNum(const int& maxKeyNum) const;
    bool isSubKeyNumOverKeyLength(const std::vector<int>& vKeyLength) const;
    bool empty() const {
        return m_keyNum == -1 ? true : false;
    }

    void dump(std::ostream& ofs = std::cout) const;
    friend std::ostream& operator << (std::ostream& ofs, const KeyNum& keyNum) {
        keyNum.dump(ofs);
        return ofs;
    }

    bool operator == (const KeyNum& k) const {
        return m_keyNum == k.getKeyNum() && m_subKeyNum == k.getSubKeyNum() ? true : false;
    }
    bool operator != (const KeyNum& k) const {
        return m_keyNum!=k.getKeyNum() || m_subKeyNum!=k.getSubKeyNum() ? true : false;
    }

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.simple(m_keyNum);
        stream.simple(m_subKeyNum);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.simple(m_keyNum);
        stream.simple(m_subKeyNum);
    }
};

class KeyRange
{
public:
    KeyNum m_startKeyNum;
    KeyNum m_endKeyNum;
    bool m_isRange;

public:
    KeyRange() {
        m_isRange = false;
    }
    bool isRange() const {
        return m_isRange;
    }
    const KeyNum& getStartKeyNum() const {
        return m_startKeyNum;
    }
    KeyNum& getStartKeyNum() {
        return m_startKeyNum;
    }
    void setStartKeyNum(KeyNum& keyNum) {
        m_startKeyNum = keyNum;
    }
    const KeyNum& getEndKeyNum() const {
        return m_endKeyNum;
    }
    KeyNum& getEndKeyNum() {
        return m_endKeyNum;
    }
    void setEndKeyNum(KeyNum& keyNum) {
        m_endKeyNum = keyNum;
    }
    bool parse(const std::string& sKeyRange, ErrorData& errData);
    void dump(std::ostream& ofs = std::cout) const;
    bool ifIncludeOmissibleKeyIntSet(const std::set<int>& setOmissibleKey) const;
    bool ifIncludeKeyIntSet(const std::set<int>& setKey) const;
    bool isKeyNumOverMaxKeyNum(const int& maxKeyNum) const;
    bool isSubKeyNumOverKeyLength(const std::vector<int>& vKeyLength) const;
    bool empty() const {
        return m_startKeyNum.empty();
    }
    bool isWholeSentence() const;
    friend std::ostream& operator << (std::ostream& ofs, const KeyRange& keyRange) {
        keyRange.getStartKeyNum().dump(ofs);
        if(keyRange.isRange()) {
            ofs<<"-";
            keyRange.getEndKeyNum().dump(ofs);
        }
        return ofs;
    }

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.content(m_startKeyNum);
        stream.content(m_endKeyNum);
        stream.simple(m_isRange);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.content(m_startKeyNum);
        stream.content(m_endKeyNum);
        stream.simple(m_isRange);
    }
};

}//namespace sae

#endif //__MEAN_KEY_RANGE_H__
