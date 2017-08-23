#ifndef __SAE__SCRIPT_LIMIT_H__
#define __SAE__SCRIPT_LIMIT_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#else
#include <pthread.h>
#endif //WIN32
#include "ErrorData.h"
#include "KeyRange.h"
#include "MeanWord.h"
#include "Range.h"
#include "DataManager.h"

namespace sae
{

class ScriptLimitCell
{
private:
    KeyRange wordRange;
    std::vector<std::string> patterns;
#ifndef WIN32
    static pthread_mutex_t m_mutex_mae_script_limit_locker;
#endif //WIN32
public:
    bool match(const MeanWords& words, const RANGE& range, const DataManager& dataManager) const;
    bool parse(const std::string& str, DataManager* pDataManager, ErrorData& errData);
    void dump(std::ostream& ofs) const;

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.content(wordRange);
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            std::string j;
            stream.simple(j);
            patterns.push_back(j);
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.content(wordRange);
        int i,size = patterns.size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
            stream.simple(patterns[i]);
        }
        format.output_end_array();
    }
};

class ScriptLimit
{
private:
    std::vector<ScriptLimitCell> cells;

public:
    ScriptLimit() {};
    bool hasLimit() const;
    void dump(std::ostream& ofs) const;
    bool match(const MeanWords& words, const DataManager& dataManager, const RANGE& range) const;
    bool parse(const std::string& sCharLimit, DataManager* pDataMaanger, ErrorData& errData);

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        //cells
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            ScriptLimitCell j;
            stream.simple(j);
            cells.push_back(j);
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        //cells
        int i,size = cells.size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
            stream.simple(cells[i]);
        }
        format.output_end_array();
    }
};

}//namespace sae

#endif //__SAE__SCRIPT_LIMIT_H__
