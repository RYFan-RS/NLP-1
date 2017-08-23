#ifndef __SAE_H__
#define __SAE_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "AnalyzeEngine.h"

class ContextDictionary
{
private:
    sae::AnalyzeEngine m_engine;
public:
    int init(std::string s_basic_rule_path = std::string(), const std::string& s_special_rule_path = std::string(), const std::map<std::string, std::set<std::string> >* pWords = NULL);

    std::string getMeanFuncName(int i) {
        return "";
    }

    VMeanResult analyze(const std::string& input, sae::MeanWords& words, const std::vector<sae::ExecutePhase>& vExecuteMethod);
    VMeanResult analyzePhase(const std::string& input, sae::MeanWords& words, const sae::ExecutePhase& phase);

    std::string getRuleName(const int& id);
    int getRuleID(const std::string& name);
    std::vector<std::string> getRule(const int& id);
    std::vector<std::string> getRule(const std::string& name);
    sae::DataStorage& getExcutableData() {
        return m_engine.getExecutableData();
    }
    void setExecutableData(const sae::DataStorage& data) {
        m_engine.setExecutableData(data);
    }
    const std::vector<sae::ExecutePhase>& getExecuteMethod(const std::string& key);
    static std::string getVersion();
    std::string getDataVersion();
    const sae::DataManager& getData() {
        return m_engine.getExecutableData().getConstDataManager();
    }
	const sae::TagManager& getTagManager() const{
		return m_engine.getConstExecutableData().getConstDataManager().getConstTagManager();
	}
    std::vector<std::string> getWordsByTag(const std::string& sTag);
	std::vector<std::string> getEntryWords() const;
    std::string makeExecuteMethodByTag(const std::string& sTag);
	std::vector<std::string> getSemantics() const;

	bool IsInExternalTag(const std::string& strTagName)const;
	
private:
    void intersection(int& a1,int& a2, const int& b1, const int& b2);

public:
    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.content(m_engine);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.content(m_engine);
    }
};

#endif //__MEAN_H__
