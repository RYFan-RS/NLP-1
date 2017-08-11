#ifndef __ROBOSAY__BASE_CHAT__INPUT__H__
#define __ROBOSAY__BASE_CHAT__INPUT__H__
#include "SentenceParser/MixParser/Parser.h"
#include "SemanticAnalysis/RuleAnalysis/mean.h"
#include "Common/lib4/Definations.hpp"
#include "CustomerServiceChat/Common/Config.h"
#include "CustomerServiceChat/Result.h"
#include "Common/json/Json.hpp"

namespace robosay{
namespace base{
class VResult;
class Input{
public:
	Input();

	virtual ~Input();

	virtual bool parse(const std::string& sCmd);
		
	virtual std::string getName() const = 0;

	bool isValidName(const std::string& sName) const{
		return getName() == sName;
	}

	virtual Result* createResult();
	std::string getSemanticTag(const acl::String &sSemantic,const std::string& sInput);
public:
	json::Object 	m_RootObj;
	VResult			m_vResult;
	std::string		m_sIn;
};

}//namespace base
}//namespace robosay

#endif //__ROBOSAY__BASE_CHAT__INPUT__H__
