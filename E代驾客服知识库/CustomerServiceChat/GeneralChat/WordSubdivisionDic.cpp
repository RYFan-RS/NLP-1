#include "WordSubdivisionDic.h"
#include "Common/lib4/File.hpp"
#include "Common/lib4/Utility.hpp"
#include "Common/lib4/FileEncoder.hpp"
#include "Common/lib4/Definations.hpp"

namespace robosay{
namespace general{

/*
//const acl::FileName& filename : the fullname of WordSubdivisionDic.dic
bool CWordSubdivisionDic::Initialize(const acl::FileName& strFileNameDic)
{
	if (strFileNameDic.empty()) { return false; }

	std::string strFileNameDst = strFileNameDic;
	bool bIsSuccess = true;

#ifdef USE_ENCODE_DECODE
	strFileNameDst = strFileNameDic.getBaseName() + ".dat";
	bIsSuccess = ReadDatTypeFile(strFileNameDst);
#else
	bIsSuccess = ReadDicTypeFile(strFileNameDst);
#endif

	if (!bIsSuccess)
	{
		std::cerr<<"  |-Dictionary ["<<strFileNameDst<<"] loaded."<<std::endl;
	}
	
	return bIsSuccess;
}

bool CWordSubdivisionDic::ReadDatTypeFile(const acl::FileName& strFileName)
{
	acl::FileEncoder fileEncoder;
	std::vector<std::string> fileContent;

	if(fileEncoder.decodeFile(strFileName,fileContent) == false)
	{
		PRINT("Can't WordSubdivisionDic decode file ["<<strFileName<<"].");
		return false;
	}

	std::vector<std::string>::iterator iter = fileContent.begin();
	for (; iter!=fileContent.end(); ++iter)
	{
		AnalysisOneData(*iter);
	}
	return true;
}

bool CWordSubdivisionDic::ReadDicTypeFile(const acl::FileName& strFileName)
{
	acl::File file;

	if(file.read(strFileName) == false)
	{
		PRINT("Can't open WordSubdivisionDic dictonary ["<<strFileName<<"]")
		return false;
	}
	
	acl::File::iterator iter = file.begin();
	for (; iter!=file.end(); ++iter)
	{
		AnalysisOneData(*iter);
	}	
	return true;
}
*/
//data format : needfull subdivision word and subdivision result use '\t'
//subdivision result: use ',' 
void CWordSubdivisionDic::AnalysisOneData(const std::string& strData)
{
	if (strData.empty()) { return; }
	if ('#' == strData[0]) { return; }

	const acl::String& strRef = (const acl::String&)strData;

	std::vector<acl::String> words = strRef.separate_("\t");
	if(words.size() != 2 || words[0].empty() || words[1].empty()){ return; }

	m_dic[words[0]] = words[1].separate(",");
}	

void CWordSubdivisionDic::SubdivideWord(std::vector<std::string>& words)const
{
	if (words.empty()) { return; }

	std::vector<std::string> subdivisoinRet;
	
	std::vector<std::string>::iterator iter = words.begin();
	for (; iter!=words.end(); ++iter)
	{
		std::map<std::string, std::vector<std::string> >::const_iterator iterFind= m_dic.find(*iter);
		if (iterFind != m_dic.end())
		{
			(void)std::copy(iterFind->second.begin(), iterFind->second.end(), std::back_inserter(subdivisoinRet));
			continue;
		}
		subdivisoinRet.push_back(*iter);
	}
	
	words.swap(subdivisoinRet);
}


}
}
