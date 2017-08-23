#include "DataManager.h"
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32

namespace sae
{

DataManager::DataManager()
{
    gp_CellCache = &m_cellCache;
    gp_VCellCache = &m_vcellCache;
}

DataManager::~DataManager()
{

}

bool DataManager::checkReferOfIndexAndExecuteMethods(std::vector<ErrorData>& vErrData) const
{
    const std::vector<ExecutePhase>& vExecuteMethod = m_executeManager.getAllExecuteMethod();
    size_t i;

    if(m_indexManager.ifAllIndexExecuteRulesHaveBeenUsed(vExecuteMethod,vErrData) == false) {
        for(i=0; i<vErrData.size(); i++) {
            vErrData[i].errorTrace(__FILE__,__LINE__-1);
        }
        return false;
    }

    if(m_indexManager.checkEmptyReferOfIndex(vExecuteMethod,vErrData) == false) {
        for(i=0; i<vErrData.size(); i++) {
            vErrData[i].errorTrace(__FILE__,__LINE__-1);
        }
        return false;
    }

    return true;
}

void DataManager::dump(std::ostream& ofs) const
{
    ofs<<"=== WORD MAP: ==============================="<<std::endl;
    m_wordManager.dump(ofs);
    ofs<<"=== FLAG MAP: ==============================="<<std::endl;
    m_tagManager.dump(ofs);
    ofs<<"=== INDEX MAP: =============================="<<std::endl;
    m_indexManager.dump(ofs);
    //------------------------------------------//
    m_executeManager.dump(ofs);
    m_defineManager.dump(ofs);
    m_lineIndexManager.dump(ofs);
}

}//namespace sae
