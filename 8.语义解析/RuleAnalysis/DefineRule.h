#ifndef __SAE__DEFINE_RULE__
#define __SAE__DEFINE_RULE__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <map>
#include <iterator>
#include "WordManager.h"
#include "Cell.hpp"

namespace sae
{

class DefineManager;

class DefineRule
{
private:
    char m_type;
    std::vector<std::string> m_val;
    std::vector<Cell*> mc_affirmCache;
    std::vector<int> mc_affrimCacheIndex;
    std::vector<Cell*> mc_negativeCache;
    std::vector<int> mc_negativeCacheIndex;
    int  mc_lineNum;

    void makeWarningIfThereAreFunctionMarks(const std::string& value, ErrorData& errData) const;

public:
    DefineRule() {
        m_type = 0;
        mc_lineNum = -1;
    }

    const std::vector<std::string>& getVal() const {
        return m_val;
    }

    void setLineNum(const int& number) {
        mc_lineNum = number;
    }
    const int& getLineNum() const {
        return mc_lineNum;
    }

    const char& getType() const {
        return m_type;
    }
    void setType(const char& type) {
        m_type &= type;
        m_type |= type;
    }

    const std::vector<Cell*>& getAffirmCache() const {
        return mc_affirmCache;
    }
    void setAffirmCache(const std::vector<Cell*>& cache) {
        mc_affirmCache = cache;
    }
    void pushAffirmCacheCell(Cell* cell) {
        mc_affirmCache.push_back(cell);
    }

    const std::vector<Cell*>& getNegativeCache() const {
        return mc_negativeCache;
    }
    void setNegativeCache(const std::vector<Cell*>& cache) {
        mc_negativeCache = cache;
    }
    void pushNegativeCacheCell(Cell* cell) {
        mc_negativeCache.push_back(cell);
    }

    const std::vector<int>& getAffirmCacheIndex() const {
        return mc_affrimCacheIndex;
    }
    const std::vector<int>& getNegativeCacheIndex() const {
        return mc_negativeCacheIndex;
    }

    bool isNull() const {
        return m_type == 0 ? true : false;
    }

    bool hasAffirmCache() const {
        return mc_affirmCache.empty() ? false : true;
    }
    bool hasNegativeCache() const {
        return mc_negativeCache.empty() ? false : true;
    }
    void addAffirmCacheIndex(const int& index) {
        mc_affrimCacheIndex.push_back(index);
    }
    void addNegativeCacheIndex(const int& index) {
        mc_negativeCacheIndex.push_back(index);
    }

//unused:
    bool isBasicDefinition() const {
        return m_type & BASIC_DEFINITION ? true : false;
    }

public:
    bool parse(const std::string& line, DefineManager& defineManager, ErrorData& errData);
	bool preParse(const std::string& value,std::vector<std::string> &vRelatedDefine, ErrorData& errData) const;	
    bool parseConditionalDefine(const std::string& value, DefineManager& defineManager, ErrorData& errData);
    bool parseKeyDefine(const std::string& value, DefineManager& defineManager, ErrorData& errData);

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.simple(m_type);
        //m_val
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            std::string s;
            stream.simple(s);
            m_val.push_back(s);
        }
        stream.simple(mc_lineNum);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.simple(m_type);
        //m_val
        int i,size = m_val.size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
            stream.content(m_val[i]);
        }
        format.output_end_array();
        stream.simple(mc_lineNum);
    }
};

class DefineManager
{

private:
    std::map<std::string,DefineRule> m_synonymMap;
	DefineRule m_emptyRule;

private:
    bool isUniqKey(const std::string& key) const;
    std::string getRuleFromLine(const std::string& sLine) const;
    bool divideKeyAndValue(std::string& rule, std::string& key, std::string& value, ErrorData& errData) const;

public:
	bool preParse(const std::string& sDefine,ErrorData& errData,std::string &sName,std::vector<std::string> &vRelatedDefine ) const;
    bool parse(const LINE& line, const std::string& sType, ErrorData& errData);
    DefineRule& find(const std::string& key) {
		std::map<std::string,DefineRule>::iterator iter = m_synonymMap.find(key);
        return iter==m_synonymMap.end() ? m_emptyRule : iter->second;
    }
    const DefineRule& constFind(const std::string& key) const {
		std::map<std::string,DefineRule>::const_iterator iter = m_synonymMap.find(key);
        return iter==m_synonymMap.end() ? m_emptyRule : iter->second;
    }
    void dump(std::ostream& ofs = std::cout) const;

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        //m_synonymMap
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            std::string key;
            DefineRule val;
            stream.simple(key);
            stream.content(val);
            m_synonymMap.insert(std::make_pair(key, val));
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        //m_synonymMap
        int size = m_synonymMap.size();
        std::map<std::string,DefineRule>::iterator ite;
        Format &format = stream.get_format();
        format.output_start_array(size);
        for (ite = m_synonymMap.begin(); ite != m_synonymMap.end(); ite++) {
            stream.simple(ite->first);
            stream.content(ite->second);
        }
        format.output_end_array();
    }
};

}//namespace sae

#endif //__SAE__DEFINE_RULE__
