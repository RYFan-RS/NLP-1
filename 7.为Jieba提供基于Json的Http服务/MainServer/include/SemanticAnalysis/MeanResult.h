#ifndef __SAE__RESULT_H__
#define __SAE__RESULT_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "ErrorData.h"
#include "DataManager.h"
#include "NormalRule.h"
#include "Range.h"
#include "MeanWord.h"
#include "EntryManager.h"

class MeanResult
{
private:
    sae::NormalRuleManager* mp_NormalRuleManager;
    sae::DataManager* mp_DataManager;
public:
    sae::ENTRY m_ruleEntry;
    int  m_start;
    int  m_end;
    sae::VRANGE m_keyRanges;
    std::vector<std::vector<int> >  m_tagsPerWord;
    std::vector<std::vector<int> >  m_deletedTagsPerWord;
    std::vector<std::string> m_actionStrings;

    std::vector<int> m_sentenceTags;
    std::vector<std::string> m_sTemplates;
    std::vector<std::vector<std::string> > m_sAnswerTemplates;

public:
    MeanResult();
    ~MeanResult();
    void setPointersForDump(const sae::NormalRuleManager* pNormalRuleManager, const sae::DataManager& dataManager) {
        mp_NormalRuleManager = const_cast<sae::NormalRuleManager*>(pNormalRuleManager);
        mp_DataManager = const_cast<sae::DataManager*>(&dataManager);
    }
    const sae::VRANGE& getKeyRanges() const {
        return m_keyRanges;
    }
    std::string getRuleName();
    int getRuleID() {
        return m_ruleEntry.m_index;
    }
    int getRuleLineNum();
    void setRuleEntry(const sae::ENTRY& entry);
    void calculateRange();
    void dump(std::ostream& ofs = std::cout) const;
    const std::vector<std::string> getActionStrings() {
        return m_actionStrings;
    }
    const std::string dumpTag2Str(const bool& color = false);
    bool hasTagRule() const {
        return mp_NormalRuleManager->getRule(m_ruleEntry.m_index).hasTagRule();
    }
    void clear();
    sae::TagRule& getTagRule();
    std::string getTagNameAt(const int& i, const int& j);
    void setKeyRanges(const sae::VRANGE& vRange);
    void addKeyRange(const sae::RANGE& range);
    std::string getLineIndexInfo();
    void setSentenceTags(const std::vector<int>& tags);
	std::multimap<std::string,std::string> getTagContentMap(const sae::MeanWords& words) const;
	std::string getRuleIdName() const;
};

class LoopInfo
{
public:
    sae::MeanWords m_words;
    int m_start; //index the MeanResult in VMeanResult
    int m_end;
public:
    LoopInfo() {
        m_start=-1;
        m_end=-1;
    }
    void dump(std::ostream& ofs = std::cout) const;
};

class PhaseInfo : public std::vector<LoopInfo>
{
public:
    std::string m_executeMethod;
    int m_start;//=m_start of first LoopInfo
    int m_end;//=m_end of last LoopInfo
    std::string m_phaseString;
public:
    PhaseInfo() {
        m_start=-1;
        m_end=-1;
    }
    void dump(std::ostream& ofs = std::cout) const;
};

class VPhaseInfo : public std::vector<PhaseInfo>
{
public:
    void dump(std::ostream& ofs = std::cout) const;
};

class VMeanResult : public std::vector<MeanResult>
{
public:
    VPhaseInfo m_phases;
public:
    VPhaseInfo getVPhaseInfo() {
        return m_phases;
    }
	
    void dump(std::ostream& ofs = std::cout) const;
	std::multimap<std::string,std::string> getTagContentMap(const sae::MeanWords& words) const;
	void getRuleIdNameVec(std::vector<std::string>& vRuleIdName) const	;
};

#endif //__SAE__RESULT_H__
