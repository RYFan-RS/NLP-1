#include "SmtcKeyWordMngr.h"

namespace sae
{

void CSmtcKeyWordMngr::AddSmtcKeyWord(const std::string& strSmtc, const std::string& strKeyWord)
{
	(void)m_smtcKeyWord[strSmtc].AddKeyWord(strKeyWord);
}

void CSmtcKeyWordMngr::MatchSmtcAndKeyWord(const CKeyWordManager& keyWordMngr, const TagManager& tagMngr)
{
	std::map<std::string,int>::const_iterator iter;
	std::map<std::string,int> tagNameIndeNum = tagMngr.getTagMapWithAttribute("输出");

	for(iter=tagNameIndeNum.begin(); iter!=tagNameIndeNum.end(); ++iter){
		const CKeyWordSet* pKeyWordSet = NULL;
		if (!keyWordMngr.FindKeyWordSetByTagIndexNum(iter->second, pKeyWordSet)) { continue; }

		//bind(&CKeyWordManager::HandleOneKeyWordSet,this,_1,&ofs)
		pKeyWordSet->ForEach(bind(&CSmtcKeyWordMngr::AddSmtcKeyWord,this,iter->first,_1));
	}
}

}

