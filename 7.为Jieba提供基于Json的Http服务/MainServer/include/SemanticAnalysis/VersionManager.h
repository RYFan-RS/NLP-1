#ifndef __SAE_VERSION_MANAGER_H__
#define __SAE_VERSION_MANAGER_H__
#include "ErrorData.h"
#ifdef __ENCRYPT_DATA__
#include "lib4/Encoder.hpp"
#endif
#include "objio.h"
#include "giop.h"
#include "graphio.h"

namespace sae
{

class VersionManager
{
private:
    std::vector<std::string> m_version;

private:
    std::string changeToStringVersion() const;

public:
    void setVersion(const std::string& version);
    std::string getVersion() const;
    bool addFileString(const std::string& filename, const std::string& sWholeFile);

    template <class Format, class Refs>
    inline void composite(obj_input<Format, Refs>& stream) {
        size_t i,size = 0;
        Format &format = stream.get_format();
        format.input_start_array(size);
        for(i = 0; i < size; i++) {
            std::string s;
            stream.simple(s);
#ifdef __ENCRYPT_DATA__
            m_version.push_back(acl::Encoder::decode(s));
#else
            m_version.push_back(s);
#endif
        }
    }

    template <class Format, class Refs>
    inline void composite(obj_output<Format, Refs>& stream) {
        size_t i,size = m_version.size();
        Format &format = stream.get_format();
        format.output_start_array(size);
        for(i = 0; i < size; i++) {
#ifdef __ENCRYPT_DATA__
            stream.simple(acl::Encoder::encode(m_version[i]));
#else
            stream.simple(m_version[i]);
#endif        
        }
        format.output_end_array();
    }
};

}//namespace sae

#endif //__MEAN_VERSION_MANAGER_H__
