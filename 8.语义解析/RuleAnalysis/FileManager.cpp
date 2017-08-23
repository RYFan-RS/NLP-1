#include "FileManager.h"
#ifdef WIN32
#pragma warning(disable: 4786)
#include <string>
#else
#include <sys/time.h>
#endif //WIN32

namespace sae
{

bool FileManager::isIncludeLine(const std::string& s) const
{
    if(s[0] == '(') {
        if(s.find("#include") == 1 || s.find("#INCLUDE") == 1) {
            return true;
        }
    } else {
        if(s.find("#include") == 0 || s.find("#INCLUDE") == 0) {
            return true;
        }
    }
    return false;
}

bool FileManager::isNonUTFLine(const std::string &strIn) const
{
    int amout=0;
    int i=0,length=strIn.length();
    while(1) {
        if(i >= length)
            break;
        if(i<length-1) {
            if(strIn[i] == '/' && strIn[i+1] == '/') break;
        }
        if((strIn[i] & 0x80) == 0x00) {
            amout++;
            i++;
        } else if((strIn[i] & 0xE0) == 0xC0) {
            amout++;
            i+=2;
        } else if((strIn[i] & 0xF0) == 0xE0) {
            amout++;
            i+=3;
        } else {
#ifdef MEAN_DEBUG
            ssDebug << "We met an unUTF8 string:" << strIn << ".  ";
            acl::debug::ERRMSG(ERR_STREAM,ssDebug,acl::debug::LEVEL0);
			ssDebug.str("");
#endif //MEAN_DEBUG
            return true;
        }
    }
    return false;
}

bool FileManager::readDataFile(const std::string& fileName, int& ruleIndexID)
{
    std::string s,newFileName,sWholeFile;
    FILE file;
    LINE line;
    int count=0;
    LineIndex lineIndex;
    ErrorData errData;
    const char *utf8bom = "\xef\xbb\xbf";

    std::fstream ifp(fileName.c_str(),std::ios::in);
    if(!ifp) {
        errData.m_errMsg = Message::msg001(fileName);
        errData.errorTrace(__FILE__,__LINE__);
        m_errs.push_back(errData);
        return false;
    }

    //record file name in parser
    file.m_name = Utility ::getFileNameFromPath(fileName);

    //record file name in storage
    lineIndex.m_fileName = file.m_name;

    //record line start index number in storage
    lineIndex.m_start = ruleIndexID+1;

    while(!ifp.eof()) {
        //read one line from file and record it
        std::getline(ifp,s);

        //if first line contains BOM, skip it.
        if(count==0 && !s.substr(0,3).compare(utf8bom)) s = s.substr(3);

        count++;
        ruleIndexID++;
        s = CommonParser::normalizeTailOfTheLine(s);

        //check for non-UTF8 characters
        if(isNonUTFLine(s)==true && !s.empty()) {
			errData.m_errMsg = Message::msg002(s,fileName,count);
            errData.errorTrace(__FILE__,__LINE__);
            m_errs.push_back(errData);
            return false;
        }

        sWholeFile += s;

		//check if this is an include line
		if(isIncludeLine(s) == true) {
			std::string relativePath = Utility::getStringBetween(s,"\"","\"");
			std::string newFileName = Utility::generatePathWithRelativePath(fileName,relativePath);
			if(newFileName.empty()) {
				errData.m_errMsg = Message::msg003(s,fileName,count);
				errData.errorTrace(__FILE__,__LINE__);
				m_errs.push_back(errData);
				return false;
			}
			//record line end index number in storage
			lineIndex.m_end = ruleIndexID;

			//record  line index information in storage
			lineIndex.m_sumOfFileSegments += lineIndex.m_end - lineIndex.m_start + 1;
			m_storage.getDataManager().getLineIndexManager().push_back(lineIndex);
			if(readDataFile(newFileName,ruleIndexID) == false) {
				return false;
			}
			//record line start index number in storage
			lineIndex.m_start = ruleIndexID + 1;
		} else {
			line.m_line = s;
			line.m_lineNum = ruleIndexID;
			file.m_file.push_back(line);
		}
    }
    ifp.close();
    m_files.push_back(file);
    //record line end index number in storage
    lineIndex.m_end = ruleIndexID;
    //record  line index information in storage
    if(lineIndex.m_start <= lineIndex.m_end) {
        m_storage.getDataManager().getLineIndexManager().push_back(lineIndex);
    }

    //add file to data version manager
    if( m_storage.getVersionManager().addFileString(fileName,sWholeFile) == false) {
        errData.m_errMsg = Message::msg004(fileName);
		errData.errorTrace(__FILE__,__LINE__);
        m_errs.push_back(errData);
    }

    return true;
}

bool FileManager::parse()
{
    if(m_storage.parse(m_files,m_errs) == false) {
        return false;
    } else {
        return true;
    }
}

void FileManager::dumpFileData(const std::string& msg, std::ostream& ofs) const
{
    if(!msg.empty()) {
        ofs<<msg<<std::endl;
    }
}

void FileManager::dumpErrorData(const std::string& msg, std::ostream& ofs)
{
    if(!msg.empty()) {
        ofs<<msg<<std::endl;
    }
    for(size_t i=0; i<m_errs.size(); i++) {
        m_errs[i].dump(m_files,m_storage.getDataManager().getConstLineIndexManager(),ofs);
    }
}

void FileManager::dumpStorage(const std::string& msg, std::ostream& ofs) const
{
    if(!msg.empty()) {
        ofs<<msg<<std::endl;
    }
    m_storage.dump(ofs);
}

void FileManager::initDataPointer()
{
    m_storage.distributeDataManagerPointer();
}

bool FileManager::makeStorage(const std::string& fileName)
{
    Serialization serialManager(fileName);

    if( !serialManager.writeModule(m_storage.getDataManager().getTagManager(),"TagManager") ) {
        ErrorData errData;
        errData.m_errMsg = Message::msg005();
        errData.errorTrace(__FILE__,__LINE__);
        m_errs.push_back(errData);
        return false;
    }
    if( !serialManager.writeModule(m_storage,"Storage") ) {
        ErrorData errData;
        errData.m_errMsg = Message::msg006();
        errData.errorTrace(__FILE__,__LINE__);
        m_errs.push_back(errData);
        return false;
    }

    if( !serialManager.writeAllocationTable() ) {
        ErrorData errData;
        errData.m_errMsg = Message::msg007();
        errData.errorTrace(__FILE__,__LINE__);
        m_errs.push_back(errData);
        return false;
    }

    return true;
}

}//namespace sae
