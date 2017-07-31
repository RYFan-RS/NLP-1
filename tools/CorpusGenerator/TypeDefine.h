#ifndef __SAE_TYPE_DEFINE_H__
#define __SAE_TYPE_DEFINE_H__
#ifdef WIN32
#pragma warning(disable: 4786)
#endif //WIN32
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <map>
namespace sae
{

enum FILE_DATA_TYPE {
	COMMON_DEFINATION = 0,
	BUSINESS_DEFINATION,
	VARIABLE_DEFINATION,
	RULE_DEFINATION
};

typedef struct _LINE {
	int m_lineNum;
	std::string m_line;
	std::string m_defination_excepiton;
} LINE;

class FILE
{
public:
	FILE_DATA_TYPE m_type;
	std::string m_name;
	std::vector<LINE> m_file;
	std::vector<std::string> m_corpus;
};


#ifdef WIN32
#define BRIGHT_GREEN ""
#define BRIGHT_RED ""
#define RESET_COLOR ""
#else
#define BRIGHT_GREEN "\033[1;32m"
#define BRIGHT_RED "\033[1;31m"
#define RESET_COLOR "\033[0m"
#endif

}//namespace sae

#endif//__MEAN_TYPE_DEFINE_H__
