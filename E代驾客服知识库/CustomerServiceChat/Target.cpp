#include "CustomerServiceChat/Target.h"

using namespace robosay::base;

Target::Target(){
}

Target::~Target(){
	m_vFromTo.clear();
}

bool Target::empty() const{
	return true;
}

bool Target::parse(const acl::String& s){
	if(s.empty()) return false;
	return true;
}

std::string Target::toStr() const{
	std::stringstream ss;
	return ss.str();
}

