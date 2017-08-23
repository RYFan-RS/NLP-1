#ifndef __SAE__EXECUTE_RULE_H__
#define __SAE__EXECUTE_RULE_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <map>
#include <set>
#include <iterator>
#include "IndexRule.h"
#include "ExecutePhase.h"

namespace sae
{

class ExecuteRule
{
public:
    std::string m_name;
    std::vector<ExecutePhase> m_executeMethod;
    int mc_lineNum;

public:
    ExecuteRule();
    const std::string& getRuleName() const {
        return m_name;
    }
    const std::vector<ExecutePhase>& getExecuteMethod() const {
        return m_executeMethod;
    }
    template<class DataManager>
    bool parse(const std::string& s, DataManager* pDataManager,ErrorData& errData) {
        std::vector<std::string> vElement = CommonParser::divideElements(s,ExecuteMethodRuleElementsNumber,errData);
        if(vElement.empty()) {
            ERROR_MESSAGE( "The format of execute rule string `"+s+"' is wrong." )
        }
        m_name = vElement[1];

        std::vector<std::string> v = Utility::divide(vElement[2],";");
        v.pop_back();
        if( Utility::hasEmptyCell(v) > -1 ) {
            ERROR_MESSAGE( "The format of execute rule string `"+s+"' is wrong." )
        }
        for(size_t i=0; i<v.size(); i++) {
            ExecutePhase phase;
            ERROR_TRACE( phase.parse(v[i]+";",pDataManager,errData) )
            m_executeMethod.push_back(phase);
        }

        return true;
    }

    void dump(std::ostream& ofs = std::cout) const;


    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        int i,size=0;
        stream.simple(m_name);
        stream.simple(mc_lineNum);
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            ExecutePhase phase;
            stream.content(phase);
            m_executeMethod.push_back(phase);
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.simple(m_name);
        stream.simple(mc_lineNum);

        int i,size = m_executeMethod.size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i <size; ++i) {
            stream.content(m_executeMethod[i]);
        }
        format.output_end_array();
    }
};

class ExecuteManager
{

private:
    std::map<std::string,ExecuteRule> m_rules;

public:
    ExecuteManager(){};

    template<class DataManager>
    bool parse(const LINE& line, DataManager* pDataManager,ErrorData& errData) {
        std::string newLine;
        if(line.m_line[0] == '(') {
            newLine = line.m_line.substr(1,line.m_line.length()-2);
        } else {
            newLine = line.m_line;
        }

        ExecuteRule rule;

        ERROR_TRACE( rule.parse(newLine,pDataManager,errData) )

        if(m_rules.find(rule.getRuleName()) != m_rules.end()) {
            ERROR_MESSAGE( "The execute method name `"+rule.getRuleName()+"' has already been used." )
        }

        m_rules.insert(std::make_pair(rule.m_name,rule));

        return true;
    }

    template<class DataManager>
    const std::vector<ExecutePhase> dynamicParse(const std::string& s, const DataManager& dataManager) const {
        std::vector<ExecutePhase> executeMethod;

        std::vector<std::string> v = Utility::divide(s,";");

        for(size_t i=0; i<v.size(); i++) {
            ExecutePhase phase;
            ErrorData errData;
            if( phase.parse(v[i],const_cast<DataManager*>(&dataManager),errData) == false ) {
                executeMethod.clear();
                return executeMethod;
            }
            executeMethod.push_back(phase);
        }

        return executeMethod;
    }
    std::vector<ExecutePhase> getAllExecuteMethod() const;
    const std::vector<ExecutePhase>& getExecuteMethod(const std::string& name) const;

    void dump(std::ostream& ofs = std::cout) const;

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            std::string first;
            ExecuteRule second;
            stream.simple(first);
            stream.content(second);
            m_rules.insert(std::make_pair(first,second));
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        int size = m_rules.size();
        std::map<std::string,ExecuteRule>::iterator i;
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = m_rules.begin(); i != m_rules.end(); ++i) {
            stream.simple(i->first);
            stream.content(i->second);
        }
        format.output_end_array();
    }
};

}//namespace sae

#endif//__SAE__EXECUTE_RULE_H__
