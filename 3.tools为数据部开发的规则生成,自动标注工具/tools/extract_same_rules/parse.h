#ifndef __EXTRACT_RULE__
#define __EXTRACT_RULE__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <vector>
#include "Utility.h"

class ExtractRule
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

#endif //__EXTRACT_RULE__

