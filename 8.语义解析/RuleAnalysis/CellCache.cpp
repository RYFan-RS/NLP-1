#include "CellCache.h"

namespace sae
{

CellCache* gp_CellCache = NULL;

CellCache::CellCache()
{
}

CellCache::~CellCache()
{
    for(size_t i=0; i<this->size(); i++) {
        delete (*this)[i];
    }
}

}
