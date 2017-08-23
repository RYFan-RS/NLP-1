#ifndef __SAE_ERROR_DATA_H__
#define __SAE_ERROR_DATA_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "TypeDefine.h"
#include "Utility.h"
#include "LineIndexManager.h"
#include "Message.hpp"

namespace sae
{

#define	ERROR_TRACE(X)\
if(X == false){\
	errData.errorTrace(__FILE__,__LINE__);\
	return false;\
}

#define ERROR_MESSAGE(X)\
	errData.m_errMsg = X;\
	errData.errorTrace(__FILE__,__LINE__);\
	return false;

#define ADD_TO_FRONT	1
#define ADD_TO_BACK	2
#define ERROR_MESSAGE_TRACE(X,ACTION)\
	if(ACTION == ADD_TO_FRONT) errData.m_errMsg = X + errData.m_errMsg;\
	else errData.m_errMsg += X;\
	errData.errorTrace(__FILE__,__LINE__);\
	return false;

class ErrorData
{
public:
    int  m_fileIndex;
    int  m_lineIndex;
    std::string m_errMsg;
    std::vector<std::pair<const char*,int> > m_errorTrace;
    std::string m_warnMsg;
    std::vector<std::pair<const char*,int> > m_warnTrace;
public:
    ErrorData() {
        m_fileIndex=-1;
        m_lineIndex=-1;
    }
    void errorTrace(const char* fileName, const int& lineNum);
    void warnTrace(const char* fileName, const int& lineNum);
    LINE getLineData(const std::vector<FILE>& files) const;
    std::string getFileName(const std::vector<FILE>& files) const;
    void dump(const std::vector<FILE>& files, const LineIndexManager& lineIndexManager,
              std::ostream& ofs = std::cout) const;
    void dumpErrorMessage(const std::vector<FILE>& files, const LineIndexManager& lineIndexManager,
                          std::ostream& ofs = std::cout) const;
    void dumpWarnMessage(const std::vector<FILE>& files, const LineIndexManager& lineIndexManager,
                         std::ostream& ofs = std::cout) const;
    void clear();
};

}//namespace sae

#endif//__MEAN_ERROR_DATA_H__
