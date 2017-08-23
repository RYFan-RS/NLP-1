#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include "BidirectionalMap.h"

namespace sae
{

const int BidirectionalMap::ErrorCode = -1;

BidirectionalMap::BidirectionalMap()
{
    m_failStr = "";
    m_failCode = -1;
    m_autoIndexNumber=-1;
    databaseType = OTHER_DATABASE;
}

bool BidirectionalMap::importMap(const std::map<std::string,int>& mapSI)
{
    std::map<std::string,int>::const_iterator i;

    for(i=mapSI.begin(); i!=mapSI.end(); ++i) {
        m_mapSI.insert(std::make_pair(i->first,i->second));
        m_mapIS.insert(std::make_pair(i->second,i->first));
    }

    return true;
}

bool BidirectionalMap::importMap(const std::map<int,std::string>& mapIS)
{
    std::map<int,std::string>::const_iterator i;

    for(i=mapIS.begin(); i!=mapIS.end(); ++i) {
        m_mapIS.insert(std::make_pair(i->first,i->second));
        m_mapSI.insert(std::make_pair(i->second,i->first));
    }

    return true;
}

void BidirectionalMap::dump(std::ostream& ofs) const
{
    std::map<std::string,int>::const_iterator i;
    for(i=m_mapSI.begin(); i!=m_mapSI.end(); ++i) {
        ofs<<i->first<<"----"<<std::dec<<i->second<<std::endl;
    }
}

const int& BidirectionalMap::find(const std::string& s) const
{
    std::map<std::string,int>::const_iterator i = m_mapSI.find(s);

    return i == m_mapSI.end() ? m_failCode : i->second;
}

const std::string& BidirectionalMap::find(const int& i) const
{
    std::map<int,std::string>::const_iterator ite = m_mapIS.find(i);

    return ite == m_mapIS.end() ? m_failStr : ite->second;
}

const int& BidirectionalMap::insertFind(const std::string& s)
{
    std::map<std::string,int>::const_iterator i = m_mapSI.find(s);

    if(i == m_mapSI.end()) {
        insert(s);
        return m_autoIndexNumber;
    } else {
        return i->second;
    }
}

bool BidirectionalMap::insert(const std::pair<std::string,int>& pSI)
{
    m_mapSI.insert(pSI);
    m_mapIS.insert(std::make_pair(pSI.second,pSI.first));

    return true;
}

bool BidirectionalMap::insert(const std::pair<int,std::string>& pIS)
{
    m_mapIS.insert(pIS);
    m_mapSI.insert(std::make_pair(pIS.second,pIS.first));

    return true;
}

bool BidirectionalMap::insert(const std::string& word)
{
    m_mapSI.insert(std::make_pair(word,++m_autoIndexNumber));
    m_mapIS.insert(std::make_pair(m_autoIndexNumber,word));

    return true;
}

const int& BidirectionalMap::getLastIndexNumber() const
{
    return m_autoIndexNumber;
}

size_t BidirectionalMap::size() const
{
    return m_mapSI.size();
}

void BidirectionalMap::setDatabaseType(const DATABASE_TYPE& type)
{
    databaseType = type;
}

const DATABASE_TYPE& BidirectionalMap::getDatabaseType() const
{
    return databaseType;
}

std::vector<std::string> BidirectionalMap::getVStrValue() const
{
    std::vector<std::string> vRet;

    for(std::map<std::string,int>::const_iterator i=m_mapSI.begin();
        i!=m_mapSI.end(); ++i) {
        vRet.push_back(i->first);
    }

    return vRet;
}

}//namespace sae
