#include "ErrorData.h"
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32

namespace sae
{

void ErrorData::errorTrace(const char* fileName, const int& lineNum)
{
    m_errorTrace.push_back(std::make_pair(fileName,lineNum));
}

void ErrorData::warnTrace(const char* fileName, const int& lineNum)
{
    m_warnTrace.push_back(std::make_pair(fileName,lineNum));
}

LINE ErrorData::getLineData(const std::vector<FILE>& files) const
{
    if(m_lineIndex > -1) {
        return files[m_fileIndex].m_file[m_lineIndex];
    } else {
        LINE emptyLine;
        emptyLine.m_line = "";
        emptyLine.m_lineNum = -1;
        return emptyLine;
    }
}

std::string ErrorData::getFileName(const std::vector<FILE>& files) const
{
    if(m_fileIndex > -1) {
        return files[m_fileIndex].m_name;
    } else {
        return "";
    }
}

void ErrorData::dumpErrorMessage(const std::vector<FILE>& files, const LineIndexManager& lineIndexManager,
                                 std::ostream& ofs) const
{
    if(ofs==std::cout) ofs<<"\033[1;31m";
    ofs<<"Error message: ";
    ofs<<m_errMsg<<std::endl;
    std::string fileName = getFileName(files);
    if(!fileName.empty()) {
        LINE line = getLineData(files);
        if(line.m_lineNum == -1) {
            ofs<<"Error in "<<fileName<<std::endl;
        } else {
            ofs<<"Error at "<<fileName<<":"<<lineIndexManager.getLineNum(line.m_lineNum)<<" =>"<<line.m_line<<std::endl;
        }
    }
    if(!m_errorTrace.empty()) {
        std::vector<std::pair<const char*,int> >::const_iterator ite;
        int count=0;
        for(ite = m_errorTrace.begin(); ite != m_errorTrace.end(); ++ite) {
            ofs<<"#TRACE "<<count++<<" at "<<ite->first<<":"<<ite->second<<std::endl;
        }
    }
    if(ofs==std::cout) ofs<<"\033[0m";
    ofs<<std::endl;
}

void ErrorData::dumpWarnMessage(const std::vector<FILE>& files, const LineIndexManager& lineIndexManager,
                                std::ostream& ofs) const
{
    if(ofs==std::cout) ofs<<"\033[0;33m";
    ofs<<"Warning message: ";
    ofs<<m_warnMsg<<std::endl;
    std::string fileName = getFileName(files);
    if(!fileName.empty()) {
        LINE line = getLineData(files);
        if(line.m_lineNum == -1) {
            ofs<<"Warn in "<<fileName<<std::endl;
        } else {
            ofs<<"Warn at "<<fileName<<":"<<lineIndexManager.getLineNum(line.m_lineNum)<<" =>"<<line.m_line<<std::endl;
        }
    }
    if(!m_warnTrace.empty()) {
        std::vector<std::pair<const char*,int> >::const_iterator ite;
        int count=0;
        for(ite = m_warnTrace.begin(); ite != m_warnTrace.end(); ++ite) {
            ofs<<"#TRACE "<<count++<<" at "<<ite->first<<":"<<ite->second<<std::endl;
        }
    }
    if(ofs==std::cout) ofs<<"\033[0m";
    ofs<<std::endl;
}

void ErrorData::dump(const std::vector<FILE>& files, const LineIndexManager& lineIndexManager,
                     std::ostream& ofs) const
{
    if(!m_errMsg.empty()) {
        dumpErrorMessage(files,lineIndexManager,ofs);
    }
    if(!m_warnMsg.empty()) {
        dumpWarnMessage(files,lineIndexManager,ofs);
    }
}

void ErrorData::clear()
{
    m_fileIndex=-1;
    m_lineIndex=-1;
    m_errMsg="";
    m_errorTrace.clear();
    m_warnMsg="";
    m_warnTrace.clear();
}

}//namespace sae
