#include "WordCombination.h"

CWordCombination::CWordCombination(char cCombinationFlag)
	:m_cCombinationFlag(cCombinationFlag)
{
	
}
void CWordCombination::Combination(const std::string& strWord)
{
	m_combinationRet.clear();

	Combination(strWord, 0, std::string(""));
}

void CWordCombination::Combination(const std::string& strWord, int iCurIndex, std::string curStr)
{
	size_t iLength = strWord.length();
	size_t i = 0;
	
	for (i=iCurIndex; i<iLength; ++i)
	{
		if (strWord[i] == m_cCombinationFlag)
		{
			break;
		}
		curStr.push_back(strWord[i]);
	}
	
	if (i == iLength)
	{
		if (!curStr.empty())
		{
			m_combinationRet.insert(curStr);
		}
		return;
	}
	
	Combination(strWord, i+1, curStr);
	curStr.push_back(m_cCombinationFlag);
	Combination(strWord, i+1, curStr);
}

