#ifndef __SAE__FLAG_RULE_H__
#define __SAE__FLAG_RULE_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "DataManager.h"
#include "KeyRange.h"
#include "TagLimit.h"

namespace sae
{

class TagRuleCell
{
public:
    KeyRange m_keyRange;
    TagLimit m_tagLimit;
public:
    void setKeyRange(const KeyRange& keyRange) {
        m_keyRange = keyRange;
    }
    const KeyRange& getKeyRange() {
        return m_keyRange;
    }
    void setTagLimit(const TagLimit& tagLimit) {
        m_tagLimit = tagLimit;
    }
    const TagLimit& getTagLimit() const {
        return m_tagLimit;
    }
    void dump(const DataManager& dataManager, std::ostream& ofs = std::cout) const;

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.content(m_keyRange);
        stream.content(m_tagLimit);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.content(m_keyRange);
        stream.content(m_tagLimit);
    }
};

class TagRule
{
private:
    std::vector<TagRuleCell> m_tagSets;
public:
    const std::vector<TagRuleCell>& getTagSets() const {
        return m_tagSets;
    };
    bool parse(const std::string& rule, DataManager* pDataManager, ErrorData& errData);
    void dump(const DataManager& dataManager, std::ostream& ofs = std::cout) const;
    bool isKeyOutOfRange(const std::vector<int>& lengthMatrix, ErrorData& errData) const;
    bool checkKeyRangesConflictWithOmissilbeKeys(const std::set<int>& setOmissibleKey);

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        //m_tags
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            TagRuleCell cell;
            stream.content(cell);
            m_tagSets.push_back(cell);
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        //m_tags
        int i,size = m_tagSets.size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
            stream.content(m_tagSets[i]);
        }
        format.output_end_array();
    }
	bool preProcess(const std::string& rule,std::set<std::string>& vTags, ErrorData& errData) const;
};

}//namespace sae

#endif //__SAE__FLAG_RULE_H__
