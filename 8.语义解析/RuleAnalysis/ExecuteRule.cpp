#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "CommonParser.h"
#include "ExecuteRule.h"

namespace sae
{

ExecuteRule::ExecuteRule()
{
    mc_lineNum = -1;
}

void ExecuteRule::dump(std::ostream& ofs) const
{
    ofs<<m_name<<"\t ExecuteRule WILL BE DUMP HERE"<<std::endl;
}

const std::vector<ExecutePhase>& ExecuteManager::getExecuteMethod(const std::string& name) const
{
    std::map<std::string,ExecuteRule>::const_iterator i = m_rules.find(name);
    return i->second.getExecuteMethod();
}

void ExecuteManager::dump(std::ostream& ofs) const
{
    ofs<<"=== EXECUTE METHOD: ========================="<<std::endl;
    std::map<std::string,ExecuteRule>::const_iterator i;
    for(i=m_rules.begin(); i!=m_rules.end(); ++i) {
        (i->second).dump(ofs);
    }
}

std::vector<ExecutePhase> ExecuteManager::getAllExecuteMethod() const
{
    std::vector<ExecutePhase> executeMethods,allExecuteMethods;
    std::map<std::string,ExecuteRule>::const_iterator i;

    for(i=m_rules.begin(); i!= m_rules.end(); i++) {
        executeMethods = i->second.getExecuteMethod();
        for(size_t j=0; j<executeMethods.size(); j++) {
            allExecuteMethods.push_back(executeMethods[j]);
        }
    }

    return allExecuteMethods;
}

}//namespace sae
