#include "Range.h"

namespace aisms
{
namespace dypsr
{

void Range::setValue(int nBeg, int nEnd)
{
    this->first = nBeg;
    this->second = nEnd;
}

bool Range::isNull() const
{
    return first == -1 || second == -1 ? true : false;
}

int Range::getLength() const
{
    return second - first + 1;
}

std::string Range::toStr() const{
	std::stringstream ss;
	ss<<"{"<<first<<","<<second<<"}";
	return ss.str();
}

}//dypsr
}//namespace aisms
