#ifndef __SAE_DATA_STORAGE_H__
#define __SAE_DATA_STORAGE_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "DataManager.h"
#include "NormalRule.h"
#include "EntryManager.h"
#include "VersionManager.h"

namespace sae
{

class DataStorage
{
private:
    DataManager  m_dataManager;
    NormalRuleManager  m_normalRuleManager;
    EntryManager m_entryManager;
    VersionManager m_VersionManager;

    bool parseDataFile(const FILE& file, std::vector<ErrorData>& vErrData);
    std::string getRuleTypeStr(const std::string& line) const;
    std::string getRuleNameStr(const std::string& line, const std::string& end="\t") const;
    std::string getLineType(const LINE& line, ErrorData& errData);
    bool checkHeadAndTailBrackets(const std::string& line) const;
    bool checkPairBrackets(const std::string& line) const;

    std::string getLineType(const std::string& line, ErrorData& errData);
    bool ParseDefineRule(const std::string& line, std::map<int, std::vector<int> >& mUsrWords);
    bool divideKeyAndValue(std::string rule, std::string& key, std::string& value) const;
public:
    bool parseUsrDic(const acl::String& file);
    void makeEntries();
    bool addUsrWord(const std::map<std::string, std::set<std::string> >* pWords);
    void distributeDataManagerPointer();
    DataManager& getDataManager() {
        return m_dataManager;
    }
    const DataManager& getConstDataManager() const {
        return m_dataManager;
    }
    DataManager* getDataManagerPointer() {
        return &m_dataManager;
    }
    NormalRuleManager& getNormalRuleManager() {
        return m_normalRuleManager;
    }
    const NormalRuleManager& getConstNormalRuleManager() const {
        return m_normalRuleManager;
    }
    const NormalRuleManager* getNormalRuleManagerPointer() const {
        return &m_normalRuleManager;
    }
    EntryManager& getEntryManager() {
        return m_entryManager;
    }
    const EntryManager& getConstEntryManager() const {
        return m_entryManager;
    }
    EntryManager* getEntryManagerPointer() {
        return &m_entryManager;
    }

    bool initAfterSerializing();
    void dump(std::ostream& ofs = std::cout) const;
    void setTagManager(const TagManager& tagManager) {
        m_dataManager.setTagManager(tagManager);
    }
    VersionManager& getVersionManager() {
        return m_VersionManager;
    }
    const VersionManager& getConstVersionManager() const {
        return m_VersionManager;
    }

    bool parse(const std::vector<FILE>& files, std::vector<ErrorData>& vErrData);

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        //m_dataManager
        stream.content(m_dataManager);
        //m_normalRuleManager
        stream.content(m_normalRuleManager);
        //m_entryManager
        stream.content(m_entryManager);
        //m_VersionManager
        stream.content(m_VersionManager);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        //m_dataManager
        stream.content(m_dataManager);
        //m_normalRuleManager
        stream.content(m_normalRuleManager);
        //m_entryManager
        stream.content(m_entryManager);
        //m_VersionManager
        stream.content(m_VersionManager);
    }
};

}//namespace sae

#endif//__MEAN_DATA_STORAGE_H__
