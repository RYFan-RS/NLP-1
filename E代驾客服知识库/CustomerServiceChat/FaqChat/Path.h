#ifndef __ROBOSAY__FAQ_CHAT__PATH__H__
#define __ROBOSAY__FAQ_CHAT__PATH__H__

namespace robosay{
namespace faq{
class Path : public robosay::base::Path{
public:
	Path();

	virtual ~Path();

	virtual bool setPath(const void* qa);
};

}//namespace faq
}//namespace robosay

#endif //__ROBOSAY__FAQ_CHAT__CHATPATH__H__
