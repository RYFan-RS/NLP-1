#ifndef __SAE__BIDIRECTIONAL_MAP_H__
#define __SAE__BIDIRECTIONAL_MAP_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#define __ENCRYPT_DATA__
#ifdef __ENCRYPT_DATA__
#include "Common/lib4/Encoder.hpp"
#endif
#include "ErrorData.h"
#include "objio.h"
#include "giop.h"
#include "graphio.h"

namespace sae
{

class BidirectionalMap
{

private:
    std::map<std::string,int> m_mapSI;
    std::map<int,std::string> m_mapIS;
    int m_autoIndexNumber;
    DATABASE_TYPE databaseType;
public:
    std::string m_failStr;
    int m_failCode;
    static const int ErrorCode;

public:
    BidirectionalMap();

    void dump(std::ostream& ofs = std::cout) const;
    bool insert(const std::pair<std::string,int>& pSI);
    bool insert(const std::pair<int,std::string>& pIS);
    bool insert(const std::string& word);
    const int& find(const std::string& s) const;
    const std::string& find(const int& i) const;
    const int& insertFind(const std::string& s);
    size_t size() const;
    void setDatabaseType(const DATABASE_TYPE& type);
    const DATABASE_TYPE& getDatabaseType() const;
    std::vector<std::string> getVStrValue() const;
    
//unused:
    bool importMap(const std::map<std::string,int>& mapSI);
    bool importMap(const std::map<int,std::string>& mapIS);
    const int& getLastIndexNumber() const;


    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        //m_mapSI
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            std::string	key;
            int val;
            stream.simple(key);
            stream.simple(val);
#ifdef __ENCRYPT_DATA__
            m_mapSI.insert(std::make_pair(acl::Encoder::decode(key), val));
#else
            m_mapSI.insert(std::make_pair(key, val));
#endif 
        }
        //m_mapIS
        Format &format1 = stream.get_format();
        format1.input_start_array(size);
        for(i = 0; i < size; i++) {
            int	key;
            std::string val;
            stream.simple(key);
            stream.simple(val);
#ifdef __ENCRYPT_DATA__
            m_mapIS.insert(std::make_pair(key, acl::Encoder::decode(val)));
#else
            m_mapIS.insert(std::make_pair(key, val));
#endif 
        }
        stream.simple(m_autoIndexNumber);
        int iEnum;
        stream.simple(iEnum);
        databaseType = (DATABASE_TYPE)iEnum;
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        //m_mapSI
        int size = m_mapSI.size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        std::map<std::string,int>::iterator ite;
        for (ite = m_mapSI.begin(); ite != m_mapSI.end(); ite++) {
#ifdef __ENCRYPT_DATA__
            stream.simple(acl::Encoder::encode(ite->first));
#else
            stream.simple(ite->first);
#endif 
            stream.simple(ite->second);
        }
        format.output_end_array();
        //m_mapIS
        size = m_mapIS.size();
        std::map<int,std::string>::iterator ite1;
        Format &format1 = stream.get_format();
        format1.output_start_array(size);
        for (ite1 = m_mapIS.begin(); ite1 != m_mapIS.end(); ite1++) {
            stream.simple(ite1->first);
#ifdef __ENCRYPT_DATA__
            stream.simple(acl::Encoder::encode(ite1->second));
#else
            stream.simple(ite1->second);
#endif 
        }
        format1.output_end_array();
        stream.simple(m_autoIndexNumber);
        stream.simple((int)databaseType);
    }
};

}//namespace sae

#endif //__SAE__BIDIRECTIONAL_MAP_H__
