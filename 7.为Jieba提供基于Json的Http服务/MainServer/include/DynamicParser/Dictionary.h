#ifndef __AISMS__DYPSR__DICTIONARY__H__
#define __AISMS__DYPSR__DICTIONARY__H__
#include "lib4/FileEncoder.hpp"
#include "Result.h"

namespace aisms
{
namespace dypsr
{

class Dictionary
{
public:
    bool init(const std::string& filename);
	bool initWithEncryption(const std::string& filename);
    bool init(const std::vector<std::string>& vWord);
    Range find(const Result& result, size_t pos) const;
	std::vector<std::string> getWords() const;
private:
    std::vector<std::string> makeCharFromStr(const acl::String& s) const;
private:
    std::multimap<std::string,std::vector<std::string> > m_sFirstChar_vCharactor;
};

}//dypsr
}//namespace aisms

#endif //__AISMS__DYPSR__DICTIONARY__H__
