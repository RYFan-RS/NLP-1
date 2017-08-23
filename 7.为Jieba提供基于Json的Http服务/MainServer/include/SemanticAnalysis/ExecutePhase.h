#ifndef __SAE_EXECUTE_PHASE_H__
#define __SAE_EXECUTE_PHASE_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <set>
#include "VCell.hpp"

namespace sae
{

class ExecutePhase
{

private:
    VCell conditionTags;
public:
    int start;
    int end;
    int loopTime;

private:
    std::string cutConditionString(std::string& s) const;
    bool parseExecutePhaseString(const std::string& s, ErrorData& errData);
    template<class DataManager>
    bool parseConditionString(const std::string& s, DataManager* pDataManager,ErrorData& errData) {
        if(s.empty()) return true;

        ERROR_TRACE( conditionTags.parse(s,pDataManager->getDefineManager(),pDataManager->getTagManager(),errData) )

        return true;
    }
public:
    ExecutePhase();
    bool matchConditon(MatchingMatrix& matrix) const;
    bool hasCondition() const;
    template<class DataManager>
    bool parse(const std::string& s, DataManager* pDataManager,ErrorData& errData) {
        std::string sLine = CommonParser::removeCommentAtTail(s);

        std::string sCondition = cutConditionString(sLine);
        if( parseConditionString(sCondition,pDataManager,errData) == false ) {
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }

        if(parseExecutePhaseString(sLine,errData) == false) {
            errData.errorTrace(__FILE__,__LINE__-1);
            return false;
        }

        return true;
    }

    template<class DataManager>
    void dump(const DataManager& dataManager,std::ostream& ofs) const {
        if(!conditionTags.empty()) {
            ofs<<"(";
            for(size_t i=0; i<conditionTags.size(); i++) {
                if(i != 0) ofs<<"/";
                conditionTags[i]->dump(dataManager.getConstTagManager(),ofs);
            }
            ofs<<")";
        }

        ofs<<start<<"-"<<end<<":"<<loopTime<<";";
    }
    template<class DataManager>
    std::string dump2Str(const DataManager& dataManager) const {
        std::stringstream ss;
        dump(dataManager,ss);
        return ss.str();
    }

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.simple(start);
        stream.simple(end);
        stream.simple(loopTime);
        stream.content(conditionTags);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.simple(start);
        stream.simple(end);
        stream.simple(loopTime);
        stream.content(conditionTags);
    }
};

}//namespace sae

#endif //__MEAN_EXECUTE_PHASE_H__
