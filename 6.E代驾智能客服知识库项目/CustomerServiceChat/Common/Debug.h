#ifndef __ROBOSAY__FAQ_CHAT__DEBUG__H__
#define __ROBOSAY__FAQ_CHAT__DEBUG__H__
#include <iostream>
#include <sstream>
#include "Common/GlobalOption.h"

#ifdef _DEBUG
#include "Common/lib4/Debug.hpp"
#include "Common/lib4/System.hpp"
#include "Common/lib4/FileName.hpp"
#include "Common/lib4/ConfigFile.hpp"
#endif //_DEBUG

namespace robosay{

extern std::string DSTREAM;
extern std::stringstream ssDebug;

#ifdef _DEBUG
extern acl::ConfigFile DEBUG_CONFIG;
#endif //_DEBUG

}//namespace robosay

#endif //__ROBOSAY__FAQ_CHAT__DEBUG__H__