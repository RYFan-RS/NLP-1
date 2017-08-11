#ifndef __WORD__SUBDIVISION__DIC__FZ__H__
#define __WORD__SUBDIVISION__DIC__FZ__H__

#include "Common/lib4/FileName.hpp"
#include "CustomerServiceChat/DicAnalysis.h"

#include <map>
#include <vector>

namespace robosay{
namespace general{

class CWordSubdivisionDic : public base::CDicAnalysis
{
public:
	//bool Initialize(const acl::FileName& strFileNameDic);
	void SubdivideWord(std::vector<std::string>& words)const;

private:
	//bool ReadDatTypeFile(const acl::FileName& strFileName);
	//bool ReadDicTypeFile(const acl::FileName& strFileName);

protected:
	virtual void AnalysisOneData(const std::string& strData);
	
private:
	//first : needfull subdivision word;	second : subdivision result
	std::map<std::string, std::vector<std::string> > m_dic;
};

}
}

#endif

