#ifndef __AISMS__DYPSR__RULE_OF_PARSE__H__
#define __AISMS__DYPSR__RULE_OF_PARSE__H__
#include "lib4/String.hpp"

namespace aisms
{
namespace dypsr
{

class RuleOfParse : public std::vector<std::string>
{
public:
    /**
        sRule : PersonalName;PlaceName;CommonDic
    */
    bool parse(const acl::String& sRule);
};

}//dypsr
}//namespace aisms

#endif //__AISMS__DYPSR__RULE_OF_PARSE__H__
