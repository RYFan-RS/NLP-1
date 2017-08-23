#ifndef __SAE__CELL_CACHE_H__
#define __SAE__CELL_CACHE_H__
#include "Cell.hpp"

namespace sae
{

class CellCache : public std::vector<Cell*>
{
public:
    CellCache();
    ~CellCache();

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        size_t i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            Cell* cell;
            stream.pointer(cell);
            this->push_back(cell);
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        size_t i,size = this->size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
            stream.pointer((*this)[i]);
        }
        format.output_end_array();
    }
};

extern CellCache* gp_CellCache;

}//namespace sae

#endif //__SAE__VCELL_CACHE_H__
