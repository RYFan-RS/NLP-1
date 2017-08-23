#ifndef __SAE__ENTRY_MANAGER_H__
#define __SAE__ENTRY_MANAGER_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "ErrorData.h"
#include "NormalRule.h"

namespace sae
{

class ENTRY
{
public:
    int       m_index;
    int       m_indexRuleID;
	int       m_fileLineIdx;

    std::string toStr() const{
        std::stringstream ss;
        ss<<"index:"<<m_index<<"ruleID:"<<m_indexRuleID<<"fileLineIdx:"<<m_fileLineIdx;
        return ss.str();
    }

public:
    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.simple(m_index);
        stream.simple(m_indexRuleID);
		stream.simple(m_fileLineIdx);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.simple(m_index);
        stream.simple(m_indexRuleID);
		stream.simple(m_fileLineIdx);
    }
};

class EntryMap : public std::multimap<int,ENTRY>
{
public:
    template<class Data, class LineIndex>
    void dump(Data& data, LineIndex& lineIndex, std::ostream& ofs = std::cout) const {
        for(std::multimap<int,ENTRY>::const_iterator i=this->begin(); i!=this->end(); ++i) {
            ofs<<data.find(i->first)<<" ---- "<<lineIndex.dumpLineIndexInfo(i->second.m_fileLineIdx)<<std::endl;
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            int	key;
            ENTRY val;
            stream.simple(key);
            stream.content(val);
            (*this).insert(std::make_pair(key, val));
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        int size = (*this).size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        std::multimap<int,ENTRY>::iterator ite;
        for (ite = (*this).begin(); ite != (*this).end(); ite++) {
            int key = ite->first;
            ENTRY val = ite->second;
            stream.simple(key);
            stream.content(val);
        }
    }
};

class EntryManager
{
private:
    EntryMap m_wordEntries;
    EntryMap m_tagEntries;
    EntryMap m_tagRuleIdxMap;
    NormalRuleManager* mp_normalRuleManager;
    DataManager* mp_dataManager;

public:
    void setNormalRuleManagerPointer(NormalRuleManager* pNormalRuleManager, DataManager* pDataManager) {
        mp_normalRuleManager = pNormalRuleManager;
        mp_dataManager = pDataManager;
    }
    const std::multimap<int,ENTRY>& getWordEntries() const {
        return m_wordEntries;
    }
    const std::multimap<int,ENTRY>& getTagEntries() const {
        return m_tagEntries;
    }
    const std::multimap<int,ENTRY>& getTagRuleIdxMap() const {
        return m_tagRuleIdxMap;
    }
    void dump(std::ostream& ofs = std::cout) const;
    bool makeEntries();
    void clearEntries();

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.content(m_wordEntries);
        stream.content(m_tagEntries);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.content(m_wordEntries);
        stream.content(m_tagEntries);
    }
};

}//namespace sae

#endif //__SAE__ENTRY_MANAGER_H__
