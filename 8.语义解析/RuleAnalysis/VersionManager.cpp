#include "VersionManager.h"
#include "Utility.h"
#include "md5.h"

namespace sae
{

void VersionManager::setVersion(const std::string& version)
{
    m_version = Utility::divide(version,"|");
}

std::string VersionManager::getVersion() const
{
    return changeToStringVersion();
}

std::string VersionManager::changeToStringVersion() const
{
    std::string s;

    for(size_t i=0; i<m_version.size(); i++) {
        if(i != 0) s += "|";
        s += m_version[i];
    }

    return s;
}

bool VersionManager::addFileString(const std::string& filename, const std::string& sWholeFile)
{
    char* md5String = MD5String(sWholeFile.c_str());
    m_version.push_back(Utility::getFileNameFromPath(filename) + ":" + md5String);

    return true;
}

}//namespace sae
