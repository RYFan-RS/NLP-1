#ifndef __SAE__NORMAL_RULE_H__
#define __SAE__NORMAL_RULE_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "KeyPart.h"
#include "TagRule.h"
#include "DataManager.h"

namespace sae
{

class NormalRule
{
private:
    int m_idx;//index number by search IndexManager.
    KeyPart m_keys;
    TagRule m_tagRule;
    int mc_lineIndexID;//file line number
    DataManager* mp_DataManager;
	std::string m_idName;
private:
    std::string getName() const;
    bool checkWholePartElementsNumber(const std::string& rule, ErrorData& errData);
    bool getNameFromString(std::string& rule, std::string& name, ErrorData& errData);
    bool getKeyFromString(std::string& rule, std::string& sKey, const int& keyIndex, ErrorData& errData);
    const std::string getRuleType(const std::string& rule) const;
    bool checkKeyRangesInExecuteRulesConflictWithOmissibleKeys(const std::set<int>& setOmissibleKey, ErrorData& errData);

public:
    NormalRule() {
        mp_DataManager=NULL;
        m_idx = -1;
    }
    NormalRule(DataManager* pDataManager) {
        mp_DataManager=pDataManager;
        m_idx = -1;
    }
    void setDataManagerPointer(DataManager* pData) {
        mp_DataManager = pData;
    }
    void dump(std::ostream& ofs = std::cout) const;

    void setIdx(const int& idx) {
        m_idx = idx;
    }
    const int& getIdx() const {
        return m_idx;
    }
    void setLineIndexID(const int& num) {
        mc_lineIndexID = num;
    }
    const int& getLineIndexID() const {
        return mc_lineIndexID;
    }
    std::string getLineIndexInfo() const {
        return mp_DataManager->getConstLineIndexManager().dumpLineIndexInfo(mc_lineIndexID);
    }
    KeyPart& getKeyPart() {
        return m_keys;
    }
    const KeyPart& getConstKeyPart() const {
        return m_keys;
    }

    bool addUsrDic(const std::map<int, std::vector<int> >& mUsrWords){
        return m_keys.addUsrDic(mUsrWords);
    }

    bool parse(const std::vector<std::string>& vRules, ErrorData& errData);

    bool hasTagRule() const {
        return m_tagRule.getTagSets().size();
    }

    TagRule& getTagRule() {
        return m_tagRule;
    }
	
	std::string getIdName() const{
		return m_idName;
	}

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) { 
        stream.simple(m_idx);
		stream.content(m_idName);   		
        stream.content(m_keys);
        stream.content(m_tagRule);
        stream.simple(mc_lineIndexID);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.simple(m_idx);
		stream.content(m_idName);   		
        stream.content(m_keys);
        stream.content(m_tagRule);
        stream.simple(mc_lineIndexID);
    }
	bool preProcess(const std::vector<std::string>& vRules, ErrorData& errData,std::set<std::string>& vTags, std::set<std::string>& vIndex) ;
};

class NormalRuleManager
{
private:
    std::vector<NormalRule> m_rules;//storage all rule infos
public:
    void setDataManagerPointer(DataManager* pData);
	bool parseTemplate(const LINE& line, DataManager* pDataManager, ErrorData& errData);
    bool parse(const LINE& line, DataManager* pDataManager, ErrorData& errData);
    void dump(std::ostream& ofs = std::cout) const;
    std::vector<NormalRule>& getRules();
    const std::vector<NormalRule>& getConstRules() const {
        return m_rules;
    };
    NormalRule& getRule(const int& index);
    const NormalRule& getConstRule(const int& index) const;
    bool addUsrDic(const std::map<int, std::vector<int> >& mUsrWords){
        const std::vector<NormalRule>::iterator last = m_rules.end();
        std::vector<NormalRule>::iterator itr = m_rules.begin();
        for(; last != itr; ++itr){
            itr->addUsrDic(mUsrWords);
        }

        return true;
    }

protected:
    std::vector<std::string> divideRuleLine(const std::string& line);
    bool getRuleType(const std::string& sRule, ErrorData& errData);

public:
    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        //m_rules
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            NormalRule cell;
            stream.content(cell);
            m_rules.push_back(cell);
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        //m_rules
        int i,size = m_rules.size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
            stream.content(m_rules[i]);
        }
        format.output_end_array();
    }
};

}//namespace sae

#endif //__SAE__NORMAL_RULE_H__
