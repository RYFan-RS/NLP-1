#ifndef __SAE__INDEX_RULE__
#define __SAE__INDEX_RULE__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <map>
#include "BidirectionalMap.h"
#include "ExecutePhase.h"

namespace sae
{

class ExecutePhase;

class IndexManager : public BidirectionalMap
{
private:
    std::map<int,int> mc_lineMap;//first index number, second line number
    int m_maxNumberInMap;
public:
    IndexManager() {
        m_maxNumberInMap = -1;
    }
    bool parse(const LINE& line, ErrorData& errData);
    bool ifAllIndexExecuteRulesHaveBeenUsed(const std::vector<ExecutePhase>& vExecuteMethod, std::vector<ErrorData>& vErrData) const;
    bool checkEmptyReferOfIndex(const std::vector<ExecutePhase>& vExecuteMethod, std::vector<ErrorData>& vErrData) const;

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        //mc_lineMap
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            int first,second;
            stream.simple(first);
            stream.simple(second);
            mc_lineMap.insert(std::make_pair(first,second));
        }
        BidirectionalMap::composite(stream);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        //mc_lineMap
        int size = mc_lineMap.size();
        std::map<int,int>::iterator i;
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = mc_lineMap.begin(); i != mc_lineMap.end(); ++i) {
            stream.simple(i->first);
            stream.simple(i->second);
        }
        format.output_end_array();
        BidirectionalMap::composite(stream);
    }
};

}//namespace sae

#endif //__SAE__INDEX_RULE__
