#ifndef __ROBOSAY__BASE_CHAT__CHATRESPONSE__H__
#define __ROBOSAY__BASE_CHAT__CHATRESPONSE__H__

#include "Common/libxml2/XmlParser.hpp"
#include "Common/lib4/Time.hpp"
#include "Common/lib4/FileName.hpp"
#include "Common/lib4/Definations.hpp"
#include "Common/lib4/Utility.hpp"
#include "Common/lib4/CommonDef.h"
#include "Result_info.hpp"

namespace robosay{
namespace base{

#define SETVALUE(A,B) \
		m_xmlResponse.setXPathValue(A,B);\
		m_sePathContent.insert(A);

/*#define SETCDATAVALUE(A,B) \
		m_xmlResponse.setXPathCDataValue(A,B);\
		m_sePathContent.insert(A);
*/

class ChatResponse
{
public:
	ChatResponse();
	~ChatResponse();
	bool init(const acl::FileName& sFileName) ;

    std::string generateResponse(const robosay::XmlParser& xmlRequest, const Result_info& resultInfo);

private:
	void copySamePathNodes(xmlNodePtr cur);
	void removeCommentNodes(xmlNodePtr cur);
	bool resetNodes();
    bool SETCDATAVALUE(std::string sPath, std::string sValue);

private:
	robosay::XmlParser m_xmlResponse;
	StringSet m_sePathContent;
	acl::FileName m_sFileName;
};

}//namespace base
}//namespace robosay
#endif //__ROBOSAY__BASE_CHAT__CHATRESPONSE__H__

