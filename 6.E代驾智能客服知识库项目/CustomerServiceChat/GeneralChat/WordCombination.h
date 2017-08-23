#ifndef __WORD_COMBINATION__FZ__H__
#define __WORD_COMBINATION__FZ__H__

#include <algorithm>
#include <set>
#include <string>

class CWordCombination
{
public:
	CWordCombination(char cCombinationFlag);
	
	void Combination(const std::string& strWord);

private:
	void Combination(const std::string& strWord, int iCurIndex, std::string curStr);

public:
	template<class OutputIterator>
	void CopyResult(OutputIterator outIter)const
	{
		(void)std::copy(m_combinationRet.begin(),m_combinationRet.end(),outIter);
	}

	//void (*)(const std::string&)
	template<class CallBackFunc>
	void ForEach(CallBackFunc callBackFunc)const
	{
		(void)std::for_each(m_combinationRet.begin(),m_combinationRet.end(),callBackFunc);
	}	

private:
	char m_cCombinationFlag;
	std::set<std::string> m_combinationRet;
};

#endif

