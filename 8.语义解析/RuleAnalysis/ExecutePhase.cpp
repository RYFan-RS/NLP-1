#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <stdlib.h>
#include "ExecutePhase.h"
#include "CommonParser.h"

namespace sae
{

ExecutePhase::ExecutePhase()
{
    start = 0;
    end = 0;
    loopTime = 0;
}

bool ExecutePhase::parseExecutePhaseString(const std::string& s, ErrorData& errData)
{
    std::vector<std::string> v;
    size_t pos1=std::string::npos,pos2=std::string::npos,pos3=s.length();
    size_t i;

    for(i=0; i<s.length(); i++) {
        if(s[i] == '-') {
            v.push_back(s.substr(0,i));
            pos1 = i+1;
        } else if(s[i] == ':') {
            v.push_back(s.substr(pos1,i-pos1));
            pos2 = i+1;
        } else if(s[i] == ';') {
            pos3 = i;
        }
    }

    if(pos1 != std::string::npos && pos2 != std::string::npos && pos3 != s.length()) {
        v.push_back(s.substr(pos2,pos3-pos2));
    }

    if(v.size() != 3) {
        ERROR_MESSAGE( "The format of execute method string `"+s+"' is wrong." )
    }

    if(Utility::hasEmptyCell(v) > -1) {
        ERROR_MESSAGE( "The format of execute method string `"+s+"' is wrong." )
    }

    for(i=0; i<v.size(); i++) {
        if(Utility::isPureNumber(v[i]) == false) {
            ERROR_MESSAGE( "The string `"+v[i]+"' in execute method string `"+s+"' is not a numerical string." )
        }
    }

    start = atoi(v[0].c_str());
    end = atoi(v[1].c_str());
    loopTime = atoi(v[2].c_str());

    return true;
}

std::string ExecutePhase::cutConditionString(std::string& s) const
{
    std::string sCondition;
    int level = -1;

    if(s[0] == '(') {
        level++;
        for(size_t i=1; i<s.length(); i++) {
            if(s[i] == '(') {
                level++;
            } else if(s[i] == ')') {
                if(level == 0) {
                    sCondition = s.substr(1,i-1);
                    s = s.substr(i+1);
                }
                level--;
            }
        }
    }

    return sCondition;
}

bool ExecutePhase::matchConditon(MatchingMatrix& matrix) const
{
    return conditionTags.allMatch(matrix);
}

bool ExecutePhase::hasCondition() const
{
    return conditionTags.empty() ? false : true;
}

}//namespace sae
