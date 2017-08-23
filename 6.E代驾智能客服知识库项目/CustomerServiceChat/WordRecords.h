#ifndef __ROBOSAY__BASE__WORD__RECORDS__H__
#define __ROBOSAY__BASE__WORD__RECORDS__H__

#include <map>
#include <string>

namespace robosay{
namespace base{
	
class CWordRecords
{
public:
	void Clear()
	{
		m_recordInfos.clear();
	}
	
	void SetQuetion(const std::string& strQuetion)
	{
		m_strQuetion = strQuetion;
	}
		
	void AddRecord(const std::string& strNew, const std::string& strOriginal)
	{
		if (strNew.empty() || strOriginal.empty()) { return; }
		
		(void)m_recordInfos.insert(std::make_pair(strNew,strOriginal));
	}
	
	std::string GetQustionWithTag(const StringStringMap& semanticInfoMap)const
	{
		std::string strQueWithTag = m_strQuetion;
		
		StringStringMap::const_iterator iter = semanticInfoMap.begin();
		for (; iter!=semanticInfoMap.end(); ++iter)
		{
			//首先从分词后的记录中找原始信息，没有则说明分词时没有改变
			std::string strOriginal = GetOriginalByNew(iter->second);
			if (strOriginal.empty()) {
				strOriginal = iter->second;
			}

			if (strOriginal.empty()) { continue; }
			
			//std::cout << "---->" << strOriginal << std::endl << std::endl;
			size_t uPos = strQueWithTag.find(strOriginal);
			if (std::string::npos == uPos) { continue; }

			(void)strQueWithTag.insert(uPos+strOriginal.length(), "["+iter->first+"]");
		}

		return strQueWithTag;
	}	
	
private:
	const std::string& GetOriginalByNew(const std::string& strNew)const
	{
		std::map<std::string, std::string>::const_iterator iterRet = m_recordInfos.find(strNew);
		
		return m_recordInfos.end()!=iterRet ? iterRet->second : m_invalidInfo;
	}
	
private:
	const std::string 	m_invalidInfo;
	std::string			m_strQuetion;
	//first : new , second : original
	std::map<std::string, std::string> m_recordInfos;
};
	
}
}

#endif
