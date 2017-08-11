#include "DicAnalysis.h"
#include "Common/lib4/File.hpp"
#include "Common/lib4/Utility.hpp"
#include "Common/lib4/FileEncoder.hpp"
#include "Common/lib4/Definations.hpp"


namespace robosay{
namespace base{

//const acl::FileName& filename : the fullname of CDicAnalysis.dic
bool CDicAnalysis::Initialize(const acl::FileName& strFileNameDic)
{
	if (strFileNameDic.empty()) { return false; }

	std::string strFileNameDst;
	bool bIsSuccess = ReadFile(strFileNameDic,strFileNameDst);

	ReleaseMidData();
	
	if (!bIsSuccess)
	{
		std::cerr<<"  |-Dictionary ["<<strFileNameDst<<"] loaded."<<std::endl;
	}
	
	return bIsSuccess;
}

bool CDicAnalysis::ReadFile(const acl::FileName& strFileName, std::string& strFileNameDst)
{
	strFileNameDst = strFileName;
	
#ifdef USE_ENCODE_DECODE
	strFileNameDst = strFileName.getBaseName() + ".dat";
	return ReadDatTypeFile(strFileNameDst);
#else
	return ReadDicTypeFile(strFileNameDst);
#endif
}

bool CDicAnalysis::ReadDatTypeFile(const acl::FileName& strFileName)
{
	acl::FileEncoder fileEncoder;
	std::vector<std::string> fileContent;

	if(fileEncoder.decodeFile(strFileName,fileContent) == false)
	{
		PRINT("Can't CDicAnalysis decode file ["<<strFileName<<"].");
		return false;
	}

	std::vector<std::string>::iterator iter = fileContent.begin();
	for (; iter!=fileContent.end(); ++iter)
	{
		AnalysisOneData(*iter);
	}
	return true;
}

bool CDicAnalysis::ReadDicTypeFile(const acl::FileName& strFileName)
{
	acl::File file;
	
	file.setCommentMarks("#");

	if(file.read(strFileName) == false)
	{
		PRINT("Can't open CDicAnalysis dictonary ["<<strFileName<<"]")
		return false;
	}
	
	acl::File::iterator iter = file.begin();
	for (; iter!=file.end(); ++iter)
	{
		AnalysisOneData(*iter);
	}	
	return true;
}


}
}

