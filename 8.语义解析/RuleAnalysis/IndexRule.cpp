#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "CommonParser.h"
#include "Utility.h"
#include "IndexRule.h"

namespace sae
{

bool IndexManager::parse(const LINE& line, ErrorData& errData)
{
    std::string newLine;
    if(line.m_line[0] == '(') {
        newLine = line.m_line.substr(1,line.m_line.length()-2);
    } else {
        newLine = line.m_line;
    }

    std::vector<std::string> vElement = CommonParser::divideElements(newLine,IndexRuleElementsNumber,errData);
    if(vElement.empty()) {
        ERROR_MESSAGE( Message::msg101() )
    }

    if(Utility::isPureNumber(vElement[2]) == false) {
        ERROR_MESSAGE( Message::msg102() )
    }

    int number = atoi(vElement[2].c_str());
    int returnNumber = find(vElement[1]);
    if(returnNumber != m_failCode) {
        ERROR_MESSAGE( Message::msg103(vElement[1]) )
    }

    if(mc_lineMap.find(number) != mc_lineMap.end()) {
        ERROR_MESSAGE( Message::msg104(vElement[1]) )
    }

    if(number > m_maxNumberInMap) {
        if(number > RULE_INDEX_MAX_NUMBER) {
            ERROR_MESSAGE( Message::msg105(RULE_INDEX_MAX_NUMBER) )
        }
        m_maxNumberInMap = number;
    }

    insert(std::make_pair(vElement[1],number));
    mc_lineMap.insert(std::make_pair(number,line.m_lineNum));

    return true;
}

bool IndexManager::ifAllIndexExecuteRulesHaveBeenUsed(const std::vector<ExecutePhase>& vExecuteMethod, std::vector<ErrorData>& vErrData) const
{
    for(std::map<int,int>::const_iterator i=mc_lineMap.begin(); i!=mc_lineMap.end(); ++i) {
        if(i->first == 0) continue; //resever for null

        bool foundFlag = false;
        for(size_t j=0; j<vExecuteMethod.size(); j++) {
            if(vExecuteMethod[j].start <= i->first && i->first <= vExecuteMethod[j].end) {
                foundFlag = true;
                break;
            }
        }
        if(foundFlag == false) {
            ErrorData errData;
            errData.m_errMsg = Message::msg106(i->first);
            errData.errorTrace(__FILE__,__LINE__-1);
            errData.m_fileIndex = -1;
            errData.m_lineIndex = -1;
            vErrData.push_back(errData);
        }
    }

    return vErrData.empty() ? true : false;
}

bool IndexManager::checkEmptyReferOfIndex(const std::vector<ExecutePhase>& vExecuteMethod, std::vector<ErrorData>& vErrData) const
{
    for(size_t i=0; i<vExecuteMethod.size(); i++) {
        if(vExecuteMethod[i].start == vExecuteMethod[i].end && vExecuteMethod[i].start == 0) continue;//resever for null

        bool emptyFlag = true;
        for(int j=vExecuteMethod[i].start; j<=vExecuteMethod[i].end; j++) {
            if(BidirectionalMap::find(j) != BidirectionalMap::m_failStr) {
                emptyFlag = false;
                break;
            }
        }
        if(emptyFlag == true) {
            ErrorData errData;
            errData.m_errMsg = Message::msg107(vExecuteMethod[i].start,vExecuteMethod[i].end,vExecuteMethod[i].loopTime);
            errData.errorTrace(__FILE__,__LINE__-1);
            errData.m_fileIndex = -1;
            errData.m_lineIndex = -1;
            vErrData.push_back(errData);
        }
    }

    return vErrData.empty() ? true : false;
}

}//namespace sae
