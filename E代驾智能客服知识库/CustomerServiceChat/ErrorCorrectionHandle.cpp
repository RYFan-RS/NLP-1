#include "ErrorCorrectionHandle.h"

namespace robosay{
namespace base{

CErrorWordSearch::CErrorWordSearch(
	const std::vector<std::string>& normalWords, const std::vector<std::string>& errorCorrectionWords)
	:m_normalWords(normalWords),m_errorCorrectionWords(errorCorrectionWords)
{
	m_iCurCorrectLen		= 0;
	m_iNormalWordNum		= m_normalWords.size(); 
	m_iCorrectWordNum		= m_errorCorrectionWords.size();

	m_iCurNormalWordIndex	= 0;
	m_iCurCorrectWordIndex	= 0;		
}	

void CErrorWordSearch::Process()
{
	if ((m_iNormalWordNum<=1) || (m_iCorrectWordNum<=0)) { return ; }

	while ((m_iCurNormalWordIndex<m_iNormalWordNum) && (m_iCurCorrectWordIndex<m_iCorrectWordNum))
	{
		//找出第一个不相等的分词
		if (m_normalWords[m_iCurNormalWordIndex] == m_errorCorrectionWords[m_iCurCorrectWordIndex])
		{
			m_iCurCorrectLen += m_errorCorrectionWords[m_iCurCorrectWordIndex].length();
			m_iCurCorrectWordIndex++;
			m_iCurNormalWordIndex++;
			continue;
		}

		int iTempCorrectIndex = m_iCurCorrectWordIndex;
		int iHandleLen = HandlemutualContain();

		if (iTempCorrectIndex == m_iCurCorrectWordIndex)
		{
			m_correctingWords.push_back(std::make_pair(m_iCurCorrectLen,m_errorCorrectionWords[m_iCurCorrectWordIndex]));
		}
		m_iCurCorrectLen += iHandleLen;
		
		m_iCurNormalWordIndex++;
		m_iCurCorrectWordIndex++;
		
	}
}

int CErrorWordSearch::HandlemutualContain()
{
	int iCurNormalLen = m_normalWords[m_iCurNormalWordIndex].length();
	int iCurCorrectLen = m_errorCorrectionWords[m_iCurCorrectWordIndex].length();

	while (iCurNormalLen != iCurCorrectLen)
	{
		if (iCurNormalLen > iCurCorrectLen)
		{
			iCurCorrectLen += m_errorCorrectionWords[++m_iCurCorrectWordIndex].length();
			continue;
		}

		iCurNormalLen += m_normalWords[++m_iCurNormalWordIndex].length();
	}

	return iCurCorrectLen;
}

const std::vector<std::pair<int,std::string> >& CErrorWordSearch::GetCorrectingWords()const
{
	return m_correctingWords;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CErrorCorrectionHandle::Initialize(const std::string& strFileName)
{
	if (!m_errorCorrectionDic.Initialize(strFileName))
	{
		PRINT("CErrorCorrectionHandle Initialize errorCorrectionDic error!");
		return false; 
	}

	m_normalParser.initStaticParser();
	m_errorCorrectionParser.initStaticParser();

	//添加纠错词到纠错分词动态词典中
	std::vector<std::string> errorWords;
	m_errorCorrectionDic.GetErrorWords(errorWords);
	m_errorCorrectionParser.getDynamicParser().init("UserDic",errorWords);	
	
	return true;
}

void CErrorCorrectionHandle::AddKeywordToDic(const std::set<std::string>& keywords)
{
	std::vector<std::string> tmpKeywords;
	(void)std::copy(keywords.begin(), keywords.end(), std::back_inserter(tmpKeywords));
	
	m_normalParser.getDynamicParser().init("UserDic",tmpKeywords);
	m_errorCorrectionParser.getDynamicParser().init("UserDic",tmpKeywords);
}

void CErrorCorrectionHandle::CorrectError(std::string& strNeedfulCorretData)
{
	std::vector<std::string> normalWords = m_normalParser.parse(strNeedfulCorretData).getWords();
	std::vector<std::string> errorCorrectionWords = m_errorCorrectionParser.parse(strNeedfulCorretData).getWords();

	CorrectWords(strNeedfulCorretData, normalWords);
	CorrectWords(strNeedfulCorretData, errorCorrectionWords);

	//test
	std::cout << "\n***********************************************" << std::endl;
	//std::cout << "纠错前：" << strNeedfulCorretData << std::endl << std::endl;
	TestOutput(normalWords,"正常分词");
	TestOutput(errorCorrectionWords,"纠错分词");

	if (normalWords.size() <= errorCorrectionWords.size()) 
	{
		std::cout << "\n不需要纠错" << std::endl;
		std::cout << "***********************************************\n" << std::endl;
		return ; 
	}

	CErrorWordSearch errorWordSearch(normalWords,errorCorrectionWords);
	errorWordSearch.Process();

	CorrectError(strNeedfulCorretData,errorWordSearch.GetCorrectingWords());

	//test
	std::cout << "***********************************************" << std::endl;
	std::cout << "纠错后：" << strNeedfulCorretData << std::endl;
	//std::cout << "***********************************************\n" << std::endl;
	
}

void CErrorCorrectionHandle::CorrectWords(const std::string& strSrc, std::vector<std::string>& words)
{
	if (strSrc.empty() || words.empty()) { return ;}

	std::vector<std::string> newWords;
	int iStartPos = 0;

	std::vector<std::string>::const_iterator iter = words.begin();
	for (; iter!=words.end(); ++iter)
	{
		int iFindPos = strSrc.find(*iter);
		if (iStartPos < iFindPos)
		{
			newWords.push_back(strSrc.substr(iStartPos,iFindPos-iStartPos));	
			iStartPos = iFindPos;
		}

		newWords.push_back(*iter);
		iStartPos += iter->length();		
	}

	words.swap(newWords);
}

void CErrorCorrectionHandle::CorrectError(
	std::string& strNeedfulCorretData, const std::vector<std::pair<int,std::string> >& wordsCorrecteds)const
{
	int iLen = strNeedfulCorretData.length();
	
	std::string strTempRet;
	int iStartPos = 0;
		
	std::vector<std::pair<int,std::string> >::const_iterator iter = wordsCorrecteds.begin();
	for (; iter!=wordsCorrecteds.end(); ++iter)
	{
		if (iter->first >= iLen) { continue; }
		std::string strRightWord = m_errorCorrectionDic.GetRightWordByErrorWord(iter->second);
		if (strRightWord.empty()) { continue; }

		strTempRet += strNeedfulCorretData.substr(iStartPos,iter->first-iStartPos);
		iStartPos = iter->first;
		strTempRet += strRightWord;
		iStartPos += iter->second.length();
	}

	strTempRet += strNeedfulCorretData.substr(iStartPos);
	strNeedfulCorretData = strTempRet;
}


void CErrorCorrectionHandle::TestOutput(const std::vector<std::string>& words, const std::string& strLabel)const
{
	std::cout << strLabel << " : {";
	
	std::vector<std::string>::const_iterator iter = words.begin();
	for (; iter!=words.end(); ++iter)
	{
		std::cout << *iter << ',';
	}

	std::cout << "\b}" << std::endl;
}


}
}
