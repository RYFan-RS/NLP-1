#ifndef __ERROR__CORRECTION__HANDLE__FZ__H__
#define __ERROR__CORRECTION__HANDLE__FZ__H__

#include "CustomerServiceChat/ErrorCorrectionDic.h"
#include "SentenceParser/MixParser/Parser.h"


namespace robosay{
namespace base{

class CErrorWordSearch
{
public:
	CErrorWordSearch(const std::vector<std::string>& normalWords, const std::vector<std::string>& errorCorrectionWords);

public:
	void Process();

	const std::vector<std::pair<int,std::string> >& GetCorrectingWords()const;

private:
	int HandlemutualContain();
		
private:
	const std::vector<std::string>& m_normalWords;
	const std::vector<std::string>& m_errorCorrectionWords;

	//first:start index, second:correctingWord
	std::vector<std::pair<int,std::string> > m_correctingWords;

private:
	int m_iCurCorrectLen;	//当前校正字符长度
	int m_iNormalWordNum; //标准的分词个数
	int m_iCorrectWordNum;//校正的分词个数

	int m_iCurNormalWordIndex;
	int m_iCurCorrectWordIndex;
	
};


class CErrorCorrectionHandle
{
public:
	bool Initialize(const std::string& strFileName);

	void AddKeywordToDic(const std::set<std::string>& keywords);

	void CorrectError(std::string& strNeedfulCorretData);

private:
	std::vector<std::pair<int,std::string> > GetCorrectedWord(
		const std::vector<std::string>& normalWords,const std::vector<std::string>& errorCorrectionWords)const;
	
	void CorrectError(std::string& strNeedfulCorretData, const std::vector<std::pair<int,std::string> >& wordsCorrecteds)const;
	void CorrectWords(const std::string& strSrc, std::vector<std::string>& words);

	void TestOutput(const std::vector<std::string>& words, const std::string& strLabel)const;
	
private:
	CErrorCorrectionDic m_errorCorrectionDic;
	aisms::mxpsr::Parser m_normalParser;
	aisms::mxpsr::Parser m_errorCorrectionParser;
};

}
}

#endif
