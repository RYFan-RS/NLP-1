#ifndef __CHAT__WORD__DISTANCE__H__
#define __CHAT__WORD__DISTANCE__H__

#include <string>

#include <vector>

namespace robosay{
namespace base{

class WordDistance
{
public:
	WordDistance();
	virtual ~WordDistance();	
	bool Initialize(const std::string& trainBinFile);

	/*word如果包含" \t\n\r" 字符，必须把这些字符去掉*/
    std::vector<std::string> getWordDistance(const std::string& word);
	
private:
  float *M;
  char *vocab;
  long long words;
  long long size;
};

}
}

#endif
