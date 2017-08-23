#ifndef __SAE__TEMPLATE_RULE__
#define __SAE__TEMPLATE_RULE__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <map>
#include "TypeDefine.h"
#include "ErrorData.h"
#ifdef __ENCRYPT_DATA__
#include "Common/lib4/Encoder.hpp"
#endif
#include "objio.h"
#include "giop.h"
#include "graphio.h"

namespace sae
{

class VectorString : public std::vector<std::string>
{
public:
	template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        size_t i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            std::string s;
            stream.simple(s);
#ifdef __ENCRYPT_DATA__
            this->push_back(acl::Encoder::decode(s));
#else
            this->push_back(s);
#endif
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        size_t i,size = this->size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
#ifdef __ENCRYPT_DATA__
            stream.simple(acl::Encoder::encode(this->at(i)));
#else
            stream.simple(this->at(i));
#endif        
        }
        format.output_end_array();
    }
};

class Template
{
private:
    std::map<std::string, VectorString > m_mTemplateRules;
	
public:
    bool parse(const LINE& line, ErrorData& errData);
	bool produce(const std::string& sLine, std::vector<std::string>& vRuleLine, ErrorData& errData) const;
	
    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        int i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            std::string first;
			VectorString second;
            stream.simple(first);
            stream.content(second);
            m_mTemplateRules.insert(std::make_pair(first,second));
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        int size = m_mTemplateRules.size();
        std::map<std::string,VectorString>::iterator i;
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = m_mTemplateRules.begin(); i != m_mTemplateRules.end(); ++i) {
            stream.simple(i->first);
            stream.content(i->second);
        }
        format.output_end_array();
    }

private:
	// parse the #TEMPLATE
	bool checkWholePartElementsNumber(const std::string& sRule, ErrorData& errData) const;
	bool getRuleNameFromString(const std::string& sRule, std::string& sName, ErrorData& errData) const;
	// parse the @TEMPLATE
	bool getVariableFromString(const std::string& sRule, std::string& sVarName, std::string& sVarValue, ErrorData& errData) const;
	bool getTagPartFromString(const std::string& sRule, std::string& sTagPart, ErrorData& errData) const;
};

}//namespace sae

#endif //__SAE__TEMPLATE_RULE__
