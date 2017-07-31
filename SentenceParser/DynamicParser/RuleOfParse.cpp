#include "RuleOfParse.h"

namespace aisms
{
namespace dypsr
{

bool RuleOfParse::parse(const acl::String& sRule)
{
    std::vector<std::string> v = sRule.separate(";");
    for(size_t i=0; i<v.size(); i++) {
        this->push_back(v[i]);
    }

    if(this->empty()) {
        return false;
    }

    for(size_t i=0; i<this->size(); i++) {
        if(this->at(i).empty()) {
            return false;
        }
    }

    return true;
}

}//dypsr
}//namespace aisms
