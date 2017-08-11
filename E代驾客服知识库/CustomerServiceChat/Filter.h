#ifndef __ROBOSAY__BASE__FILTER_H__
#define __ROBOSAY__BASE__FILTER_H__
#include "Common/lib4/ConfigFile.hpp"
#include "Common/lib4/StringMap.hpp"
#include "Common/lib4/FileName.hpp"
#include "Common/lib4/Time.hpp"
#include "Common/KBConfig.h"
#include "Translator.h"

namespace robosay{
namespace base{


class Filter
// 此类属性保存的是airticket中的<sorter>标签对中的数据  形成一个map 主要在scan方法中体现实质
{
public:
	bool init(robosay::XmlParser& xml);

	std::string toStr() const;

	bool fromStr(const std::string& s);
	
	void scanFilter(const std::map<std::string, std::string>& mSemanticVariable, const Translator& translator, std::string& sHotelRelated);
	
	void scanRange(const std::map<std::string, std::string>& mSemanticVariable);
	
	void scan(const std::map<std::string, std::string>& mSemanticVariable, const Translator& translator, std::string& sHotelRelated);

	void scanFilter(const std::map<std::string, std::string>& mSemanticVariable);
	
	const acl::StringMap& getFilter() const;
	
	void reset();
	
	bool isUpdated() const;

private:
    acl::StringMap m_mFilter;
	bool m_bUpdated;
};

}//name base
}// namespace robosay

#endif //__AISMS__FILTER_H__
