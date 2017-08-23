#ifndef __WORD_NEAR_MEANING_H__
#define __WORD_NEAR_MEANING_H__
#include "Common/lib4/FileName.hpp"
#include "Common/lib4/File.hpp"
#include "Common/lib4/Utility.hpp"
#include "Common/lib4/FileEncoder.hpp"
#include <map>

namespace robosay
{
namespace general
{

static const std::string g_WordMeanRule_Id_Separator("=");
static const std::string g_WordMeanRule__Internal_Separator(";");
static const std::string g_WordMeanRule_Extenrnal_Separator(",");
	
class WordNearRule{
public:
	bool read(const acl::String& line);
	std::string getFirstChar() const;
	bool process(std::vector<std::string>& vWord, const size_t& nPos, const std::map<int,std::string>& wordIdtoStrMap) const;

	static std::string handleWordNearRule(const acl::Line& line, std::vector<WordNearRule>& vecWordNearRule);

	bool setWordId(int id){m_wordId=id;return true;}
private:
	bool filter() ;

public:
	std::vector<std::string> vFrom;
	int     m_wordId;
};

class WordNearMeaning{
public:
	bool initNormalFile(const std::string& filename);
	//bool initEncryptionFile(const acl::FileName& filename);
	bool readFile(const acl::File& file);
	bool process(std::vector<std::string>& vWord) const;
	
private:
	std::multimap<std::string, WordNearRule> m_m_FirstChar_Rule;
	std::map<int,std::string>  m_m_words;
};

}
}

#endif //__WORD_NEAR_MEANING_H__

