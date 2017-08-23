#include "LineIndexManager.h"

namespace sae
{

LineIndex::LineIndex()
{
    m_start = -1;
    m_end = -1;
    m_fileName = "";
    m_sumOfFileSegments = 0;
}

void LineIndex::dump(std::ostream& ofs) const
{
    ofs<<m_fileName<<":"<<m_start<<"-"<<m_end;
}

LineIndexManager::LineIndexManager()
{
    m_errorCode = -1;
    m_errorStr = "";
}

void LineIndexManager::dump(std::ostream& ofs) const
{
    ofs<<"=== LINE INDEX:==============================="<<std::endl;
    for(size_t i=0; i<(*this).size(); i++) {
        (*this)[i].dump(ofs);
        ofs<<std::endl;
    }
}

const LineIndex& LineIndexManager::getLineIndex(const int& LineIndexID) const
{
    for(size_t i=0; i<(*this).size(); i++) {
        if((*this)[i].m_start <= LineIndexID && (*this)[i].m_end >= LineIndexID) {
            return (*this)[i];
        }
    }
    return m_errorLineIndex;
}
const std::string& LineIndexManager::getFileName(const int& LineIndexID) const
{
    for(size_t i=0; i<(*this).size(); i++) {
        if((*this)[i].m_start <= LineIndexID && (*this)[i].m_end >= LineIndexID) {
            return (*this)[i].m_fileName;
        }
    }
    return m_errorStr;
}

int LineIndexManager::getLineNum(const int& LineIndexID) const
{
    for(size_t i=0; i<(*this).size(); i++) {
        if((*this)[i].m_start <= LineIndexID && (*this)[i].m_end >= LineIndexID) {
            return LineIndexID - (*this)[i].m_start + 1 + (*this)[i].m_sumOfFileSegments;
        }
    }
    return m_errorCode;
}
std::string LineIndexManager::dumpLineIndexInfo(const int& LineIndexID) const
{
    std::string s;

    for(size_t i=0; i<(*this).size(); i++) {
        if((*this)[i].m_start <= LineIndexID && (*this)[i].m_end >= LineIndexID) {
            s = (*this)[i].m_fileName + ":" + Utility::int2Str(getLineNum(LineIndexID));
            break;
        }
    }

    return s;
}

}//namespace sae
