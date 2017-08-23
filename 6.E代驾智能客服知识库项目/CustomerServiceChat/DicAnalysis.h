#ifndef __DIC__ANALYSIS__FZ__H__
#define __DIC__ANALYSIS__FZ__H__

#include "Common/lib4/FileName.hpp"


namespace robosay{
namespace base{

class CDicAnalysis
{
public:
	bool Initialize(const acl::FileName& strFileNameDic);

protected:
	virtual void AnalysisOneData(const std::string& strData) = 0;
	virtual void ReleaseMidData() { }
	virtual bool ReadFile(const acl::FileName& strFileName,std::string& strFileNameDst);

protected:
	bool ReadDicTypeFile(const acl::FileName& strFileName);
	
private:
	bool ReadDatTypeFile(const acl::FileName& strFileName);
};

}
}

#endif

