#ifndef __ERROR__CORRECTION__DIC__FZ__H__
#define __ERROR__CORRECTION__DIC__FZ__H__

#include "CustomerServiceChat/DicAnalysis.h"
#include <map>
#include <set>
#include <vector>

namespace robosay{
namespace base{
class CErrorCorrectionDic : public CDicAnalysis
{
public:
	void GetErrorWords(std::vector<std::string>& errorWords)const;
	std::string GetRightWordByErrorWord(const std::string& errorWord)const;

protected:
	virtual void AnalysisOneData(const std::string& strData);
	virtual void ReleaseMidData() 
	{
		m_rightWords.clear();
	}

	virtual bool ReadFile(const acl::FileName& strFileName,std::string& strFileNameDst);
	
private:
	//first : error word; second:right word
	std::map<std::string, std::string> m_errorAndRightWords;
	
	//temp , delete repetition word
	std::set<std::string> m_rightWords;
};

}
}
#endif

