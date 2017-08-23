#ifndef __AISMS__DYPSR__DICTIONARY__H__
#define __AISMS__DYPSR__DICTIONARY__H__
#include "Common/lib4/FileEncoder.hpp"
#include "Node.h"

namespace aisms
{
namespace dypsr
{

class Dictionary
{
public:
	Dictionary():m_nLastIdx(0){}
    bool init(const std::string& filename);
	bool initWithEncryption(const std::string& filename);
    bool init(const std::vector<std::string>& vWord);
    Range find(const Result& result, size_t pos) const;
	std::vector<std::string> getWords() const;
private:
    std::vector<int> makeCharFromStr(const acl::String& s);
private:
	std::map<int,Node> m_nChar_Node;
	std::map<std::string,int> m_sChar_nIdx;
	std::vector<std::string> m_vChar;
	int m_nLastIdx;
};

}//dypsr
}//namespace aisms

#endif //__AISMS__DYPSR__DICTIONARY__H__
