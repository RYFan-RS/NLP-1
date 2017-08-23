#ifndef __SAE__RULE_INDEX_MANAGER_H__
#define __SAE__RULE_INDEX_MANAGER_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
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
};

}//namespace sae

#endif //__SAE__RULE_INDEX_MANAGER_H__
