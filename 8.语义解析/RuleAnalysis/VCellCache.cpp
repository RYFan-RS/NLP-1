#include "VCellCache.h"

namespace sae
{

VCellCache* gp_VCellCache = NULL;

VCellCache::VCellCache()
{
}

VCellCache::~VCellCache()
{
    for(size_t i=0; i<this->size(); i++) {
        delete (*this)[i];
    }
}

int VCellCache::find(const std::string& s, const DATABASE_TYPE& type) const
{
    const std::string key = generateKey(s,type);
    std::map<std::string,int>::const_iterator ite = m_index.find(key);
    return ite == m_index.end() ? -1 : ite->second;
}

int VCellCache::insert(const std::string& s, VCell* vcell, const DATABASE_TYPE& type)
{
    const int index = this->size();
    this->push_back(vcell);
    const std::string key = generateKey(s,type);
    m_index.insert(std::make_pair(key,index));
    return index;
}

std::string VCellCache::generateKey(const std::string& s, const DATABASE_TYPE& type) const
{
    if(type == WORD_DATABASE) {
        return s+"w";
    }else {
        return s+"t";
    }
}

}
