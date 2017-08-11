#include "CustomerServiceChat/ErrorCorrectionDic.h"
#include "Common/lib4/String.hpp"

namespace robosay{
namespace base{

void CErrorCorrectionDic::AnalysisOneData(const std::string& strData)
{
	acl::String strSrc(strData);
	strSrc.strip();
	if (strSrc.empty()) { return; }
	if ('#' == strSrc[0]) { return; }

	//查询 = 查寻,察询
	std::vector<acl::String> words = strSrc.separate_("=");
	if (words.size() != 2) { return; }

	words[0].strip();
	words[1].strip();
	if(words[0].empty() || words[1].empty() ){
		return ;
	}
	std::string& rightWord = words[0];
  	if (m_rightWords.end() != m_rightWords.find(rightWord)){ return; }
	(void)m_rightWords.insert(rightWord);
	
	std::vector<acl::String> errorWords = words[1].separate_(",");

	std::vector<acl::String>::iterator iter = errorWords.begin();
	for (; iter!=errorWords.end(); ++iter)
	{
		iter->strip();
		if(!iter->empty()){
			m_errorAndRightWords.insert(std::make_pair(*iter,rightWord));
		}
	}	
}

void CErrorCorrectionDic::GetErrorWords(std::vector<std::string>& errorWords)const
{
	std::map<std::string, std::string>::const_iterator iter = m_errorAndRightWords.begin();
	for (; iter!=m_errorAndRightWords.end(); ++iter)
	{
		errorWords.push_back(iter->first);
	}
}

std::string CErrorCorrectionDic::GetRightWordByErrorWord(const std::string& errorWord)const
{
	std::map<std::string, std::string>::const_iterator iter = m_errorAndRightWords.find(errorWord);
	if (m_errorAndRightWords.end() == iter)
	{
		return std::string();
	}

	return iter->second;
}

bool CErrorCorrectionDic::ReadFile(const acl::FileName& strFileName,std::string& strFileNameDst)
{
	strFileNameDst = strFileName;
	return ReadDicTypeFile(strFileNameDst);
}


}
}

