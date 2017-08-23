#ifndef __ROBOSAY__GENERAL_CHAT__PATH__H__
#define __ROBOSAY__GENERAL_CHAT__PATH__H__

namespace robosay{
namespace general{
class Path : public robosay::base::Path{
public:
	Path();

	virtual ~Path();

	virtual bool setPath(const void* qa);
};

}//namespace general
}//namespace robosay

#endif //__ROBOSAY__GENERAL_CHAT__CHATPATH__H__
