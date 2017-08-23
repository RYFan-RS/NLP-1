#ifndef __SAE__VCELL_CACHE_H__
#define __SAE__VCELL_CACHE_H__
#include "VCell.hpp"

namespace sae
{

class VCellCache : public std::vector<VCell*>
{
private:
    std::map<std::string,int> m_index;
public:
    VCellCache();
    ~VCellCache();

    int find(const std::string& s, const DATABASE_TYPE& type) const;
    int insert(const std::string& s, VCell* vcell, const DATABASE_TYPE& type);

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        size_t i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            VCell* vcell;
            stream.pointer(vcell);
            this->push_back(vcell);
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
private:
    std::string generateKey(const std::string& s, const DATABASE_TYPE& type) const;
};

extern VCellCache* gp_VCellCache;

}//namespace sae

#endif //__SAE__VCELL_CACHE_H__
