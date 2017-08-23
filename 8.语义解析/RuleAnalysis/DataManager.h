#ifndef __SAE_DATA_MANAGER_H__
#define __SAE_DATA_MANAGER_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "Template.h"
#include "WordManager.h"
#include "TagManager.h"
#include "IndexRule.h"
#include "DefineRule.h"
#include "ExecuteRule.h"
#include "LineIndexManager.h"
#include "CellCache.h"
#include "VCellCache.h"
#include "KeyWordManager.h"

namespace sae
{

class DataManager
{
private:
	Template m_template;
    WordManager m_wordManager;
    TagManager m_tagManager;
    IndexManager m_indexManager;
    DefineManager m_defineManager;
    ExecuteManager m_executeManager;
    LineIndexManager m_lineIndexManager;
    CellCache m_cellCache;
    VCellCache m_vcellCache;
	CKeyWordManager m_keyWordManager;

public:
    DataManager();
    ~DataManager();
    void dump(std::ostream& ofs = std::cout) const;
	
	Template& getTemplate() {
		return m_template;
	}
	const Template& getConstTemplate() const{
		return m_template;
	}
    WordManager& getWordManager() {
        return m_wordManager;
    }
    const WordManager& getConstWordManager() const {
        return m_wordManager;
    }
    TagManager& getTagManager() {
        return m_tagManager;
    }
    const TagManager& getConstTagManager() const {
        return m_tagManager;
    }
    IndexManager& getIndexManager() {
        return m_indexManager;
    }
    const IndexManager& getConstIndexManager() const {
        return m_indexManager;
    }
    DefineManager& getDefineManager() {
        return m_defineManager;
    }
    const DefineManager& getConstDefineManager() const {
        return m_defineManager;
    }
    ExecuteManager& getExecuteManager() {
        return m_executeManager;
    }
    const ExecuteManager& getConstExecuteManager() const {
        return m_executeManager;
    }
    LineIndexManager& getLineIndexManager() {
        return m_lineIndexManager;
    }
    const LineIndexManager& getConstLineIndexManager() const {
        return m_lineIndexManager;
    }

	CKeyWordManager& GetKeyWordManager(){
		return m_keyWordManager;
	}

	const CKeyWordManager& GetConstKeyWordManager()const{
		return m_keyWordManager;
	}

    void setTagManager(const TagManager& tagManager) {
        m_tagManager = tagManager;
    }
    bool checkReferOfIndexAndExecuteMethods(std::vector<ErrorData>& vErrData) const;

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.content(m_wordManager);
        stream.content(m_indexManager);
        stream.content(m_defineManager);
        stream.content(m_executeManager);
        stream.content(m_lineIndexManager);
        stream.content(m_keyWordManager);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.content(m_wordManager);
        stream.content(m_indexManager);
        stream.content(m_defineManager);
        stream.content(m_executeManager);
        stream.content(m_lineIndexManager);
		stream.content(m_keyWordManager);
    }
};

}//namespace sae

#endif//__MEAN_DATA_MANAGER_H__
