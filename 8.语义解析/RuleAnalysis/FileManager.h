#ifndef __SAE_FILE_MANAGER_H__
#define __SAE_FILE_MANAGER_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "TypeDefine.h"
#include "ErrorData.h"
#include "DataStorage.h"
#include "Serialization.hpp"
#include "AnalyzeEngine.h"

namespace sae
{

class FileManager
{
private:
    std::vector<FILE> m_files;
    std::vector<ErrorData> m_errs;
    DataStorage m_storage;

    bool isNonUTFLine(const std::string &strIn) const;
    bool isIncludeLine(const std::string& s) const;

public:
    FileManager() {}
    ~FileManager() {
    }

    void dumpFileData(const std::string& msg = "", std::ostream& ofs = std::cout) const;
    void dumpErrorData(const std::string& msg = "", std::ostream& ofs = std::cout);
    void dumpStorage(const std::string& msg = "", std::ostream& ofs = std::cout) const;

    bool readDataFile(const std::string& fileName, int& ruleIndexID);
    bool parse();
    void initDataPointer();
    const std::vector<FILE>& getFilesData() const {
        return m_files;
    }
    const std::vector<LINE>& getFileData(const std::string& fileName) const;
    const std::vector<ErrorData>& getErrorData() const {
        return m_errs;
    }
    bool makeStorage(const std::string& fileName);
    const DataStorage& getStorage() const {
        return m_storage;
    }
    bool hasErrorMessage() const {
        return m_errs.empty() ? false : true;
    }
};

}//namespace sae

#endif//__MEAN_FILE_MANAGER_H__
