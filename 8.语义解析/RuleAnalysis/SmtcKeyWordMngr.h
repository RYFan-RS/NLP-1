#ifndef __SAE__SMTC__KEY__WORD_MNGR_FZ__
#define __SAE__SMTC__KEY__WORD_MNGR_FZ__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32

#include "KeyWordManager.h"
#include "TagManager.h"

namespace sae
{

class CSmtcKeyWordMngr
{
public:
	void AddSmtcKeyWord(const std::string& strSmtc, const std::string& strKeyWord);

	//in para : 
	//	strSmtc ->semantic
	//	backCallFunc -> likeness: void (*)(const std::string)
	template<class BackCallFunc>
	void ForEachBySemantic(const std::string& strSmtc, BackCallFunc backCallFunc)const
	{
		std::map<std::string, CKeyWordSet>::const_iterator iter = m_smtcKeyWord.find(strSmtc);

		if (iter == m_smtcKeyWord.end()) { return; }

		iter->second.ForEach(backCallFunc);
	}

	void MatchSmtcAndKeyWord(const CKeyWordManager& keyWordMngr, const TagManager& tagMngr);

	void Clear() { m_smtcKeyWord.clear(); }
	
private:
	std::map<std::string, CKeyWordSet> m_smtcKeyWord;		
};

}

#endif

