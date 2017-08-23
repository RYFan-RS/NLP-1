#ifndef __SAE__TAG_MANAGER_H__
#define __SAE__TAG_MANAGER_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <map>
#include "BidirectionalMap.h"
#include "ErrorData.h"

namespace sae
{

class TagManager : public BidirectionalMap
{
private:
    std::multimap<std::string,int> m_attributeMap;//first attribute string, second tag nubmer
    std::map<int,int> mc_lineMap;//first tag number, second line number
public:
    TagManager();
    bool parse(const LINE& line, ErrorData& errData);
    std::set<int> getTagSetWithAttribute(const std::string attribute);

    const std::multimap<std::string, int>& getAttributeMap() const {
        return m_attributeMap;
    }

    std::string toStr() const;

    std::map<std::string,int> getTagMapWithAttribute(const std::string attribute) const;

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        //m_attributeMap
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            std::string first;
            int second;
            stream.simple(first);
            stream.simple(second);
#ifdef __ENCRYPT_DATA__
            m_attributeMap.insert(std::make_pair(acl::Encoder::decode(first),second));
#else
            m_attributeMap.insert(std::make_pair(first,second));
#endif
        }
        //mc_lineMap
        size = 0;
        Format &format1 = stream.get_format();
        format1.input_start_array(size);
        for(i = 0; i < size; i++) {
            int first,second;
            stream.simple(first);
            stream.simple(second);
            mc_lineMap.insert(std::make_pair(first,second));
        }
        BidirectionalMap::composite(stream);
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        //m_attributeMap
        int size = m_attributeMap.size();
        std::multimap<std::string,int>::iterator i;
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = m_attributeMap.begin(); i != m_attributeMap.end(); ++i) {
#ifdef __ENCRYPT_DATA__
            stream.simple(acl::Encoder::encode(i->first));
#else
            stream.simple(i->first);
#endif
            stream.simple(i->second);
        }
        format.output_end_array();
        //mc_lineMap
        size = mc_lineMap.size();
        std::map<int,int>::iterator ii;
        Format &format1 = stream.get_format();
        format1.output_start_array(size);
        for(ii = mc_lineMap.begin(); ii != mc_lineMap.end(); ++ii) {
            stream.simple(ii->first);
            stream.simple(ii->second);
        }
        format1.output_end_array();
        BidirectionalMap::composite(stream);
    }
};

}//namespace sae

#endif //__SAE__TAG_MANAGER_H__
