#ifndef __SAE__RULE_INDEX_MANAGER_H__
#define __SAE__RULE_INDEX_MANAGER_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "objio.h"
#include "giop.h"
#include "graphio.h"
#include <iostream>
#include <vector>
#include "Utility.h"

namespace sae
{

class LineIndex
{
public:
    int m_start;
    int m_end;
    std::string m_fileName;
    int m_sumOfFileSegments;

public:
    LineIndex();
    ~LineIndex() {};
    void dump(std::ostream& ofs = std::cout) const;

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.simple(m_start);
        stream.simple(m_end);
        stream.simple(m_fileName);
        stream.simple(m_sumOfFileSegments);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.simple(m_start);
        stream.simple(m_end);
        stream.simple(m_fileName);
        stream.simple(m_sumOfFileSegments);
    }
};

class LineIndexManager : public std::vector<LineIndex>
{

public:
    LineIndex m_errorLineIndex;
    int m_errorCode;
    std::string m_errorStr;

public:
    LineIndexManager();
    ~LineIndexManager() {}
    void dump(std::ostream& ofs = std::cout) const;
    const LineIndex& getLineIndex(const int& LineIndexID) const;
    const std::string& getFileName(const int& LineIndexID) const;
    int getLineNum(const int& LineIndexID) const;
    std::string dumpLineIndexInfo(const int& LineIndexID) const;

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            LineIndex cell;
            stream.content(cell);
            (*this).push_back(cell);
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        int i,size = (*this).size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
            stream.content((*this)[i]);
        }
        format.output_end_array();
    }
};

}//namespace sae

#endif //__SAE__RULE_INDEX_MANAGER_H__
