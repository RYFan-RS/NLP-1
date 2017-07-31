#ifndef __AISMS__DYPSR__RANGE__H__
#define __AISMS__DYPSR__RANGE__H__
#include <list>
#include <iterator>
#include "Common/lib4/Definations.hpp"
#include "Common/lib4/ConfigFile.hpp"
#include "Common/lib4/FileName.hpp"
#ifdef _DYNAMIC_PARSER_DEBUG
#include "Common/lib4/Debug.hpp"
#endif

namespace aisms
{
namespace dypsr
{

#ifdef _DYNAMIC_PARSER_DEBUG
const std::string DSTREAM = "DYNAMIC_PARSER_DEBUG_LOG.log";
static std::stringstream ssDebug;
#endif

class Range
{
public:
    Range() {
        first = -1;
        second = -1;
    }
	Range(int a, int b){
		first = a;
		second = b;
	}

    void setValue(int nBeg, int nEnd);
    bool isNull() const;
    int getLength() const;
	std::string toStr() const;
public:
    int first;
    int second;
};

}//dypsr
}//namespace aisms

#endif //__AISMS__DYPSR__RANGE__H__
