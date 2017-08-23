#ifndef __SAE__ALLOCATION_TABLE_HPP__
#define __SAE__ALLOCATION_TABLE_HPP__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <map>
#include "objio.h"
#include "giop.h"
#include "graphio.h"

namespace sae
{

class AllocationRange
{
public:
    int beg;
    int end;

public:
    AllocationRange() {
        beg = -1;
        end = -1;
    }

    AllocationRange(const int& beg, const int& end) {
        this->beg = beg;
        this->end = end;
    }

    bool operator == (const AllocationRange& other) const {
        return (other.beg != beg || other.end != end) ? false : true;
    }

    void dump(std::ostream& ofs = std::cout) const {
        ofs<<"["<<beg<<","<<end<<"]";
    }

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        stream.simple(beg);
        stream.simple(end);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        stream.simple(beg);
        stream.simple(end);
    }
};

class AllocationTable
{
public:
    AllocationRange NullVal;

public:
    void add(const std::string& moduleName, const AllocationRange& range) {
        m_table.insert(std::make_pair(moduleName,range));
    }

    const AllocationRange& find(const std::string& moduleName) const {
        std::map<std::string,AllocationRange>::const_iterator iter = m_table.find(moduleName);
        return (iter == m_table.end()) ? NullVal : iter->second;
    }

    void dump(std::ostream& ofs = std::cout) const {
        std::map<std::string,AllocationRange>::const_iterator iter;
        for(iter=m_table.begin(); iter!=m_table.end(); ++iter) {
            ofs<<iter->first<<"--";
            iter->second.dump(ofs);
            ofs<<std::endl;
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            std::string	key;
            AllocationRange val;
            stream.simple(key);
            stream.content(val);
            m_table.insert(std::make_pair(key, val));
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        int size = m_table.size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        std::map<std::string,AllocationRange>::iterator ite;
        for (ite = m_table.begin(); ite != m_table.end(); ite++) {
            stream.simple(ite->first);
            stream.content(ite->second);
        }
        format.output_end_array();
    }
private:
    std::map<std::string,AllocationRange> m_table;
};

}//namespace sae

#endif //__SAE__ALLOCATION_TABLE_HPP__
